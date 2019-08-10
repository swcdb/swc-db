/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 */


#include "swcdb/lib/fs/Settings.h"
#include "swcdb/lib/core/comm/AppContext.h"
#include "swcdb/lib/client/Clients.h"
#include "swcdb/lib/db/Columns/RS/Columns.h"
#include "swcdb/lib/db/Columns/MNGR/Columns.h"

#include <iostream>


void SWC::Config::Settings::init_app_options(){
  init_fs_options();
}
void SWC::Config::Settings::init_post_cmd_args(){}

using namespace SWC;

void run(size_t thread_id){

    int err = Error::OK;
    bool exists = EnvFsInterface::fs()->exists(err, "nonexisting/"+std::to_string(thread_id));
    if(exists || err != Error::OK){ 
     std::cerr << "ERROR(nonexisting file) exists=" << exists << " err=" << err << "\n";
     exit(1);
    }
    err = Error::OK;
    EnvFsInterface::fs()->mkdirs(err, std::to_string(thread_id));
    if(err != Error::OK){ 
     std::cerr << "ERROR mkdirs err=" << err << "\n";
     exit(1);
    }
    err = Error::OK;
    EnvFsInterface::fs()->mkdirs(err, std::to_string(thread_id));
    if(err != Error::OK){ 
     std::cerr << "ERROR mkdirs err=" << err << "\n";
     exit(1);
    }

    err = Error::OK;
    FS::DirentList listing;
    EnvFsInterface::fs()->readdir(err, "", listing);
    if(err != Error::OK){ 
     std::cerr << "ERROR readir err=" << err << "\n";
     exit(1);
    }
    bool found = false;
    std::cout << "Dir List, sz=" << listing.size() <<  ":\n";
    for(auto dirent : listing){
      std::cout << " " << dirent.to_string();
      if(dirent.name.compare(std::to_string(thread_id)) == 0){
        found = true;
        break;
      }
    }
    if(!found){ 
     std::cerr << "ERROR readir missing expected dir=" << std::to_string(thread_id) << "\n";
     exit(1);
    }


    err = Error::OK;
    exists = EnvFsInterface::fs()->exists(err, std::to_string(thread_id));
    if(!exists || err != Error::OK){ 
     std::cerr << "ERROR(existing file) exists=" << exists << " err=" << err << "\n";
     exit(1);
    }

    err = Error::OK;
    EnvFsInterface::fs()->rmdir(err, std::to_string(thread_id));
    if(err != Error::OK){ 
     std::cerr << "ERROR(rmdir) err=" << err << "\n";
     exit(1);
    }
    err = Error::OK;
    exists = EnvFsInterface::fs()->exists(err, std::to_string(thread_id));
    if(exists || err != Error::OK){ 
     std::cerr << "ERROR(rmdir failed) exists=" << exists << " err=" << err << "\n";
     exit(1);
    }
    err = Error::OK;
    EnvFsInterface::fs()->remove(err, std::to_string(thread_id));
    if(err != Error::OK && err != 2){ 
     std::cerr << "ERROR(remove) non-existing err=" << err << "\n";
     exit(1);
    }
  
    err = Error::OK;
    FS::SmartFdPtr smartfd 
      = FS::SmartFd::make_ptr(
        "testfile_"+std::to_string(thread_id), FS::OpenFlags::OPEN_FLAG_OVERWRITE);
    EnvFsInterface::fs()->create(err, smartfd, -1, -1, -1);
    if(err != Error::OK || !smartfd->valid()) { 
     std::cerr << "ERROR(create) err=" << err << " " << smartfd->to_string() <<"\n";
     exit(1);
    }
    err = Error::OK;
    EnvFsInterface::fs()->close(err, smartfd);
    if(err != Error::OK){ 
     std::cerr << "ERROR(close) err=" << err << "\n";
     exit(1);
    }
    err = Error::OK;
    exists = EnvFsInterface::fs()->exists(err, smartfd->filepath());
    if(!exists || err != Error::OK){ 
     std::cerr << "ERROR(create failed) exists=" << exists << " err=" << err << "\n";
     exit(1);
    }

    err = Error::OK;
    EnvFsInterface::fs()->remove(err, smartfd->filepath());
    if(err != Error::OK){ 
     std::cerr << "ERROR(remove) created-file err=" << err << "\n";
     exit(1);
    }
    err = Error::OK;
    exists = EnvFsInterface::fs()->exists(err, smartfd->filepath());
    if(exists || err != Error::OK){ 
     std::cerr << "ERROR(remove failed) created-file  exists=" << exists << " err=" << err << "\n";
     exit(1);
    }


    // create >> append >> close >> exists >> length 
    // >> open >> read >> seek >> read(suff) >> close >> remove

    // create >> 
    EnvFsInterface::fs()->create(err, smartfd, -1, -1, -1);
    if(err != Error::OK || !smartfd->valid()) { 
     std::cerr << "ERROR(create) err=" << err << " " << smartfd->to_string() <<"\n";
     exit(1);
    }
    
    // append >> 
    std::string data("Abc1234");
    std::string data_end("567890");
    std::string data_str = data;
    int sz = 536870912-data_str.length()-data_end.length();
    for(int i=0;i<sz;i++)
      data.append("+");
    data.append(data_end);

    StaticBuffer buffer(data.data(), data.length(), false);
    size_t amount = EnvFsInterface::fs()->append(err, smartfd, buffer, FS::Flags::FLUSH);
    if(err != Error::OK || amount!=data.length() || smartfd->pos() != data.length()) { 
     std::cerr << "ERROR(append) err=" << err << " amount=" << amount << " " << smartfd->to_string() <<"\n";
     exit(1);
    }

    // close >>
    err = Error::OK;
    EnvFsInterface::fs()->close(err, smartfd);
    if(err != Error::OK){ 
     std::cerr << "ERROR(close,create) err=" << err << "\n";
     exit(1);
    }

    // exists >>
    err = Error::OK;
    exists = EnvFsInterface::fs()->exists(err, smartfd->filepath());
    if(!exists || err != Error::OK){ 
     std::cerr << "ERROR(create failed) exists=" << exists << " err=" << err << "\n";
     exit(1);
    }

    // length >>
    err = Error::OK;
    size_t len = EnvFsInterface::fs()->length(err, smartfd->filepath());
    if(err != Error::OK || len != data.length()) { 
     std::cerr << "ERROR(length) len=" << len << " expected-len=" << data.length() 
               << " err=" << err << " " << smartfd->to_string() <<"\n";
     exit(1);
    }
    
    // open >>
    err = Error::OK;
    smartfd->flags(0);
    EnvFsInterface::fs()->open(err, smartfd, -1);
    if(err != Error::OK || !smartfd->valid()) { 
     std::cerr << "ERROR(open) err=" << err << " " << smartfd->to_string() <<"\n";
     exit(1);
    }

    // read >>
    err = Error::OK;
    uint8_t buf[data_str.length()];
    if (EnvFsInterface::fs()->read(err, smartfd, buf,  data_str.length()) != data_str.length() 
        || err != Error::OK 
        || strcmp((char*)buf, data_str.c_str()) != 0) { 
     std::cerr << "ERROR(read) err=" << err << " buf=" << buf << " " << smartfd->to_string() <<"\n";
     exit(1);
    }
    std::cout << "read-data='" << std::string((char*)buf, 7) << "'\n";

    // seek >>
    err = Error::OK;
    size_t offset = len-data_end.length();
    EnvFsInterface::fs()->seek(err, smartfd, offset);
    if (err != Error::OK || smartfd->pos() != offset) { 
     std::cerr << "ERROR(seek) err=" << err << " to=" << offset << " " << smartfd->to_string() <<"\n";
     exit(1);
    }

    // read(suff) >>
    err = Error::OK;
    uint8_t bufsuf[data_end.length()];
    if (EnvFsInterface::fs()->read(err, smartfd, bufsuf,  data_end.length()) != data_end.length() 
        || err != Error::OK 
        || strcmp((char*)bufsuf, data_end.c_str()) != 0) { 
     std::cerr << "ERROR(read(suff)) err=" << err << " buf=" << bufsuf << " " << smartfd->to_string() <<"\n";
     exit(1);
    }
    std::cout << "read(suff)-data='" << std::string((char*)bufsuf, 7) << "'\n";
    

    // close >>
    err = Error::OK;
    EnvFsInterface::fs()->close(err, smartfd);
    if(err != Error::OK){ 
     std::cerr << "ERROR(close,open) err=" << err << "\n";
     exit(1);
    }

    
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main(int argc, char** argv) {
  EnvConfig::init(argc, argv);
  EnvIoCtx::init(8);

  EnvFsInterface::init();
    
  for(size_t chk=1;chk<=1;chk++) {
    std::cout << "--1--\n";
    std::vector<std::thread*> threads;
    for(size_t t=1;t<=1;t++)
      threads.push_back(new std::thread([t](){run(t);}));
    
    std::cout << "--2--\n";
    for(auto t : threads) t->join();

    std::cout << "--3--\n";
  
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "--######################-- chk=" <<chk<< "\n";
  }
  
  exit(0);
}