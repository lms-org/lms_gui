#include <data_collector.h>
#include <thread>
#include <main_window.h>
#include <lms/logging/event.h>
#include <exception>
#include <unistd.h>

DataCollector::DataCollector(MainWindow *window):m_running(false){
    //start the read thread
    mainWindow = window;
}

void DataCollector::readMessages(){
    if(!m_running){
        try{
            std::cout<<"trying to open /tmp/lms.sock"<<std::endl;
            m_client.connectUnix("/tmp/lms.sock");
            std::cout<<"opened"<<std::endl;
        }catch(std::exception e){
            std::cout<<"could not open it: "<<e.what()<<std::endl;
            usleep(1000000);
            return;
        }
    }
    m_running = true;
    m_thread = std::thread([this](){
        while(m_running){
            lms::Response res;
            lms::ProtobufSocket::Error resType;
            try{
                resType = m_client.sock().readMessage(res);
            }catch(std::exception e){
                std::cout<<"readMessage failed: "<<e.what()<<std::endl;
                continue;
            }
            if(resType == lms::ProtobufSocket::OK){
                std::lock_guard<std::mutex> myLock(resposeMutex);
                responseBuffer.push_back(res);
            }else if(resType == lms::ProtobufSocket::ERROR){
                //std::cout<<"could not read message: "<<"ERROR"<<std::endl;
                usleep(5000);
            }else if(resType == lms::ProtobufSocket::CLOSED){
                //std::cout<<"could not read message: "<<"Connection closed!"<<std::endl;
                usleep(5000);
            }else{
                //std::cout<<"could not read message: "<<"UNKOWN"<<std::endl;
                usleep(5000);
            }
        }
    });
}

void DataCollector::cycle(){
    if(!running()){
        readMessages();
        return;
    }
    //ask for runtimes every time after time
    try{
        lms::Response res;
        res.mutable_process_list();
        m_client.sock().writeMessage(res); //TODO msg only arrives once

        /*Damit killt man den master server :D
        m_client.sock().close();
        m_client.connectUnix("/tmp/lms.sock");
        */
    }catch(std::exception e){
        std::cout<<"writeMessage failed" <<e.what()<<std::endl;
        return;
    }
    parsePackages();
}

void DataCollector::parsePackages(){
    std::lock_guard<std::mutex> myLock(resposeMutex);
    if(responseBuffer.size() == 0)
        return;
    std::cout<<"parsePackage: msgcount: "<<responseBuffer.size()<<std::endl;
    for(const lms::Response &res:responseBuffer){
        std::cout<<"msg-content: "<<res.content_case()<<std::endl;
        switch (res.content_case()) {
        case lms::Response::kClientList:
        {
            mainWindow->overview->removeClients();
            for(int i = 0; i <  res.client_list().clients_size(); i++){
                mainWindow->overview->addClient(res.client_list().clients(0).peer(),res.client_list().clients(0).fd());
            }
        }
            break;
        case lms::Response::kInfo:
            std::cout<<"Inforpackage: "<<std::to_string(res.info().pid()) << ", version "<<std::to_string(res.info().version());
            break;
        case lms::Response::kLogEvent:
        {
            lms::logging::Level lvl;
            lms::Time time;
            //TODO

            mainWindow->overview->logMessage(lvl, res.log_event().tag(),res.log_event().text(),time);
        }
            break;
        case lms::Response::kModuleList:

            break;
        case lms::Response::kProcessList:
        {
            std::cout<<"got processList"<<std::endl;
            mainWindow->overview->removeProcesses();
            for(int i = 0; i <res.process_list().processes_size(); i++){
                mainWindow->overview->addProcess(res.process_list().processes(i).pid(),res.process_list().processes(i).config_file());
            }
        }
            break;
        case lms::Response::kProfilingSummary:

            break;
        default:
            break;
        }
    }
    responseBuffer.clear();
}
