/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */


#include "swcdb/core/config/Properties.h"
#include <fstream>


namespace SWC { namespace Config {

  Properties::Properties() noexcept { }

  Properties::~Properties() {
    reset();
  }

  void Properties::reset() {
    for (const auto& kv : m_map)
      delete kv.second;
    m_map.clear();
  }

  void Properties::load_from(const Config::Parser::Options& opts,
                             bool only_guarded) {
    for(const auto& kv : opts.map) {
      if(has(kv.first) && (kv.second->is_default() ||
                           (only_guarded && !kv.second->is_guarded())))
      continue;
      set(kv.first, kv.second);
    }
  }

  void Properties::load(const std::string& fname,
                        const Config::ParserConfig& filedesc,
                        const Config::ParserConfig& cmddesc,
                        bool allow_unregistered,
                        bool only_guarded) {
    Config::Parser prs(false);
    prs.config.add(filedesc);
    prs.config.add(cmddesc);

    std::ifstream in(fname.c_str());
    prs.parse_filedata(in);

    load_from(prs.get_options(), only_guarded);
    (void)allow_unregistered;
  }

  void Properties::reload(const std::string& fname,
                          const Config::ParserConfig& filedesc,
                          const Config::ParserConfig& cmddesc) {
    try {
      load(fname, filedesc, cmddesc, true);

    } catch(...) {
      const Error::Exception& e = SWC_CURRENT_EXCEPTION("");
      SWC_LOG_OUT(LOG_WARN, SWC_LOG_OSTREAM
        << "CONFIG_BAD_CFG_FILE " << fname << ": " << e;
      );
    }
  }

  void Properties::alias(const std::string& primary,
                         const std::string& secondary) {
    m_alias_map[primary] = secondary;
    m_alias_map[secondary] = primary;
  }

  void Properties::set(const std::string& name, Property::Value::Ptr p) {
    auto ptr = get_ptr(name, true);
    if(!ptr) {
      m_map.emplace(name, p->make_new());

    } else if(!p->is_default()) {
      ptr->set_from(p);
      ptr->default_value(false);
    }
  }
  bool Properties::has(const std::string& name) const noexcept {
    if(m_map.count(name))
      return true;
    auto alias = m_alias_map.find(name);
    return alias != m_alias_map.end() && m_map.count(alias->second);
  }

  bool Properties::defaulted(const std::string& name) {
    return get_ptr(name)->is_default();
  }

  std::string Properties::to_string(const std::string& name) {
    return get_ptr(name)->to_string();
  }

  void Properties::get_names(std::vector<std::string>& names) const {
    for(auto it = m_map.begin(); it != m_map.end(); ++it)
      names.push_back(it->first);
  }

  void Properties::remove(const std::string& name) {
    auto it = m_map.find(name);
    if(it != m_map.end()) {
      delete it->second;
      m_map.erase(it);
    }
  }


  Property::Value::Ptr Properties::get_ptr(const std::string& name,
                                           bool null_ok) {
    auto it = m_map.find(name);
    if (it != m_map.end())
      return it->second;

    auto alias = m_alias_map.find(name);
    if(alias != m_alias_map.end()) {
      it = m_map.find(alias->second);
      if (it != m_map.end())
        return it->second;
    }
    if(null_ok)
      return nullptr;

    SWC_THROWF(Error::CONFIG_GET_ERROR,
              "getting value of '%s' - missing",
              name.c_str());
  }

  void Properties::print(std::ostream& out, bool include_default) const {
    bool isdefault;
    for(const auto& kv : m_map) {
      isdefault = kv.second->is_default();
      if(include_default || !isdefault) {
        out << kv.first << '=' << kv.second->to_string();
        if(isdefault)
          out << " (default)";
        out << '\n';
      }
    }
  }



}}

