/*
 * Copyright (C) 2019 SWC-DB (author: Kashirin Alex (kashirin.alex@gmail.com))
 * Copyright (C) 2007-2016 Hypertable, Inc.
 *
 * This file is part of Hypertable.
 *
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or any later version.
 *
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/** @file
 * File system utility functions.
 * Helper/Utility functions for accessing files and the file system.
 */

#include "FileUtils.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <memory>

extern "C" {
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
}

#include <re2/re2.h>


using namespace SWC;

std::mutex FileUtils::ms_mutex;

bool FileUtils::read(const std::string &fname, std::string &contents) {
  off_t len {};
  char *buf = file_to_buffer(fname, &len);
  if (buf != 0) {
    contents.append(buf, len);
    delete [] buf;
    return true;
  }
  return false;
}


ssize_t FileUtils::read(int fd, void *vptr, size_t n) {
  size_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = (char *)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nread = ::read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;/* and call read() again */
      else if (errno == EAGAIN)
        break;
      else
        return -1;
    }
    else if (nread == 0)
      break; /* EOF */

    nleft -= nread;
    ptr   += nread;
  }
  return n - nleft;
}


ssize_t FileUtils::pread(int fd, off_t offset, void *vptr, size_t n) {
  ssize_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = (char *)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nread = ::pread(fd, ptr, nleft, offset)) < 0) {
      if (errno == EINTR)
        nread = 0;/* and call read() again */
      else if (errno == EAGAIN)
        break;
      else
        return -1;
    }
    else if (nread == 0)
      break; /* EOF */

    nleft -= nread;
    ptr   += nread;
    offset += nread;
  }
  return n - nleft;
}


ssize_t FileUtils::write(const std::string &fname, const std::string &contents) {
  int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    int saved_errno = errno;
    HT_ERRORF("Unable to open file \"%s\" for writing - %s", fname.c_str(),
              strerror(saved_errno));
    errno = saved_errno;
    return -1;
  }
  ssize_t rval = write(fd, contents);
  ::close(fd);
  return rval;
}


ssize_t FileUtils::write(int fd, const void *vptr, size_t n) {
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = (const char *)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = ::write(fd, ptr, nleft)) <= 0) {
      if (errno == EINTR)
        nwritten = 0; /* and call write() again */
      else if (errno == EAGAIN)
        break;
      else
        return -1; /* error */
    }

    nleft -= nwritten;
    ptr   += nwritten;
  }
  return n - nleft;
}

ssize_t FileUtils::writev(int fd, const struct iovec *vector, int count) {
  ssize_t nwritten;
  while ((nwritten = ::writev(fd, vector, count)) <= 0) {
    if (errno == EINTR)
      nwritten = 0; /* and call write() again */
    else if (errno == EAGAIN) {
      nwritten = 0;
      break;
    }
    else
      return -1; /* error */
  }
  return nwritten;
}


ssize_t FileUtils::sendto(int fd, const void *vptr, size_t n,
        const sockaddr *to, socklen_t tolen) {
  size_t nleft;
  ssize_t nsent;
  const char *ptr;

  ptr = (const char *)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nsent = ::sendto(fd, ptr, nleft, 0, to, tolen)) <= 0) {
      if (errno == EINTR)
        nsent = 0; /* and call sendto() again */
      else if (errno == EAGAIN || errno == ENOBUFS)
        break;
      else
        return -1; /* error */
    }

    nleft -= nsent;
    ptr   += nsent;
  }
  return n - nleft;
}


ssize_t FileUtils::send(int fd, const void *vptr, size_t n) {
  size_t nleft;
  ssize_t nsent;
  const char *ptr;

  ptr = (const char *)vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nsent = ::send(fd, ptr, nleft, 0)) <= 0) {
      if (errno == EINTR)
        nsent = 0; /* and call sendto() again */
      else if (errno == EAGAIN || errno == ENOBUFS)
        break;
      else
        return -1; /* error */
    }

    nleft -= nsent;
    ptr   += nsent;
  }
  return n - nleft;
}


ssize_t FileUtils::recvfrom(int fd, void *vptr, size_t n, sockaddr *from,
        socklen_t *fromlen) {
  ssize_t nread;
  while (true) {
    if ((nread = ::recvfrom(fd, vptr, n, 0, from, fromlen)) < 0) {
      if (errno != EINTR)
        break;
    }
    else
      break;
  }
  return nread;
}


ssize_t FileUtils::recv(int fd, void *vptr, size_t n) {
  ssize_t nread;
  while (true) {
    if ((nread = ::recv(fd, vptr, n, 0)) < 0) {
      if (errno != EINTR)
        break;
    }
    else
      break;
  }
  return nread;
}


bool FileUtils::set_flags(int fd, int flags) {
  int val;
  bool ret = true;

  if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
    int saved_errno = errno;
    HT_ERROR_OUT << "fcnt(F_GETFL) failed : " << ::strerror(saved_errno)
        << HT_END;
    errno = saved_errno;
    ret = false;
  }

  val |= flags;

  if (fcntl(fd, F_SETFL, val) < 0) {
    int saved_errno = errno;
    HT_ERROR_OUT << "fcnt(F_SETFL) failed : " << ::strerror(saved_errno)
        << HT_END;
    errno = saved_errno;
    ret = false;
  }

  return ret;
}


char *FileUtils::file_to_buffer(const std::string &fname, off_t *lenp) {
  struct stat statbuf;
  int fd;

  *lenp = 0;

  if ((fd = open(fname.c_str(), O_RDONLY)) < 0) {
    int saved_errno = errno;
    HT_ERRORF("open(\"%s\") failure - %s", fname.c_str(),
            strerror(saved_errno));
    errno = saved_errno;
    return 0;
  }

  if (fstat(fd, &statbuf) < 0) {
    int saved_errno = errno;
    HT_ERRORF("fstat(\"%s\") failure - %s", fname.c_str(),
           strerror(saved_errno));
    errno = saved_errno;
    return 0;
  }

  *lenp = statbuf.st_size;

  char *rbuf = new char [*lenp + 1];

  ssize_t nread = FileUtils::read(fd, rbuf, *lenp);

  ::close(fd);

  if (nread == (ssize_t)-1) {
    int saved_errno = errno;
    HT_ERRORF("read(\"%s\") failure - %s", fname.c_str(),
            strerror(saved_errno));
    errno = saved_errno;
    delete [] rbuf;
    *lenp = 0;
    return 0;
  }

  if (nread < *lenp) {
    HT_WARNF("short read (%d of %d bytes)", (int)nread, (int)*lenp);
    *lenp = nread;
  }

  rbuf[nread] = 0;

  return rbuf;
}


std::string FileUtils::file_to_string(const std::string &fname) {
  std::string str;
  off_t len;
  char *contents = file_to_buffer(fname, &len);
  str = (contents == 0) ? "" : contents;
  delete [] contents;
  return str;
}


void *FileUtils::mmap(const std::string &fname, off_t *lenp) {
  int fd;
  struct stat statbuf;
  void *map;

  if (::stat(fname.c_str(), &statbuf) != 0)
    HT_FATALF("Unable determine length of '%s' for memory mapping - %s",
            fname.c_str(), strerror(errno));
  *lenp = (off_t)statbuf.st_size;

  if ((fd = ::open(fname.c_str(), O_RDONLY)) == -1)
    HT_FATALF("Unable to open '%s' for memory mapping - %s", fname.c_str(),
            strerror(errno));
  
  if ((map = ::mmap(0, *lenp, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    HT_FATALF("Unable to memory map file '%s' - %s", fname.c_str(),
            strerror(errno));

  close(fd);
  return map;
}


bool FileUtils::mkdirs(const std::string &dirname) {
  struct stat statbuf;

  char *tmpdir = new char [dirname.length() + 1];	
  strcpy(tmpdir, dirname.c_str());	
  *(tmpdir+dirname.length()) = '/';

  int saved_errno = 0;
  for(int n=1; n < dirname.length()+1; n++){
    if(*(tmpdir+n) != '/')
      continue;
    *(tmpdir+n) = 0;

    errno = 0;
    if (stat(tmpdir, &statbuf) != 0) {
      if (errno != ENOENT) {	
        saved_errno = errno;	
        HT_ERRORF("Problem stat'ing directory '%s' - %d(%s)", tmpdir,	
                  saved_errno, strerror(saved_errno));	
        break;	
      }
      errno = 0;
      if (mkdir(tmpdir, 0755) != 0 && errno != 17) {	
        saved_errno = errno;	
        HT_ERRORF("Problem creating directory '%s' - %d(%s)", tmpdir,	
                   saved_errno, strerror(saved_errno));	
        break;	
      }	
    }
    *(tmpdir+n) = '/';
  }

  delete [] tmpdir;
  errno = saved_errno;
  return saved_errno == 0;
}


bool FileUtils::exists(const std::string &fname) {
  struct stat statbuf;
  if (stat(fname.c_str(), &statbuf) != 0)
    return false;
  return true;
}

bool FileUtils::unlink(const std::string &fname) {
  if (::unlink(fname.c_str()) == -1 && errno != 2) {
    int saved_errno = errno;
    HT_ERRORF("unlink(\"%s\") failed - %s", fname.c_str(),
              strerror(saved_errno));
    errno = saved_errno;
    return false;
  }
  return true;
}

bool FileUtils::rename(const std::string &oldpath, const std::string &newpath) {
  if (::rename(oldpath.c_str(), newpath.c_str()) == -1) {
    int saved_errno = errno;
    HT_ERRORF("rename(\"%s\", \"%s\") failed - %s",
              oldpath.c_str(), newpath.c_str(), strerror(saved_errno));
    errno = saved_errno;
    return false;
  }
  return true;
}


uint64_t FileUtils::size(const std::string &fname) {
  struct stat statbuf;
  if (stat(fname.c_str(), &statbuf) != 0)
    return 0;
  return statbuf.st_size;
}


off_t FileUtils::length(const std::string &fname) {
  struct stat statbuf;
  if (stat(fname.c_str(), &statbuf) != 0)
    return (off_t)-1;
  return statbuf.st_size;
}

time_t FileUtils::modification(const std::string &fname) {
  struct stat statbuf;
  if (stat(fname.c_str(), &statbuf) != 0)
    return 0;
  return statbuf.st_mtime;
}


void FileUtils::add_trailing_slash(std::string &path) {
  if (path.find('/', path.length() - 1) == std::string::npos)
    path += "/";
}


bool FileUtils::expand_tilde(std::string &fname) {

  if (fname[0] != '~')
    return false;

  std::lock_guard<std::mutex> lock(ms_mutex);

  struct passwd pbuf;
  struct passwd *prbuf;
  char buf[256];

  if (fname.length() == 1 || fname[1] == '/') {
    if (getpwuid_r(getuid() , &pbuf, buf, 256, &prbuf) != 0 || prbuf == 0)
      return false;
    fname = (std::string)pbuf.pw_dir + fname.substr(1);
  }
  else {
    std::string name;
    size_t first_slash = fname.find_first_of('/');

    if (first_slash == std::string::npos)
      name = fname.substr(1);
    else
      name = fname.substr(1, first_slash-1);

    if (getpwnam_r(name.c_str() , &pbuf, buf, 256, &prbuf) != 0 || prbuf == 0)
      return false;

    if (first_slash == std::string::npos)
      fname = pbuf.pw_dir;
    else
      fname = (std::string)pbuf.pw_dir + fname.substr(first_slash);
  }

  return true;
}

void FileUtils::readdir(const std::string &dirname, 
                        const std::string &fname_regex,
                        std::vector<struct dirent> &listing) {

  errno = 0;
  DIR *dirp = opendir(dirname.c_str());
  if(dirp == nullptr || errno != 0){
    HT_ERRORF("Problem reading directory '%s' - %s", dirname.c_str(),
              strerror(errno));
    return;
  }
  std::shared_ptr<re2::RE2> regex(fname_regex.length()
                                  ? new re2::RE2(fname_regex)
                                  : 0);
  struct dirent *dep;

#if defined(USE_READDIR_R) && USE_READDIR_R

  int ret;
  struct dirent de;
  for(;;) {
    if((ret = ::readdir_r(dirp, &de, &dep)) != 0 || dep == nullptr)
      break;
    if(!regex || re2::RE2::FullMatch(de.d_name, *regex))
      listing.push_back(de);
  };

#else

  for(;;) {
    if((dep = ::readdir(dirp)) == nullptr)
      break;
    if (!regex || re2::RE2::FullMatch(dep->d_name, *regex))
      listing.push_back(*dep);
  }
  
#endif

  if(errno > 0)
    HT_ERRORF("Problem reading directory '%s' - %s", dirname.c_str(),
              strerror(errno));
  (void)closedir(dirp);
}



