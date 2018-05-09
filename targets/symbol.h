#ifndef __GR8_SEMANTICS_SYMBOL_H__
#define __GR8_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>
#include <vector>

namespace gr8 {

  class symbol {
    basic_type *_type;
    std::string _name;
    int _qualifier;
    bool _function;
    std::vector<basic_type*> _param_types;
    bool _defined;
    bool _global;

  public:
    symbol(basic_type *type, const std::string &name, bool noQualifier, bool isPublic, bool isUse) :
        symbol(type, name, noQualifier, isPublic, isUse, false, std::vector<basic_type*>(0), false) {
    }

    symbol(basic_type *type, const std::string &name, bool noQualifier, bool isPublic, bool isUse, bool isFunction, std::vector<basic_type*> param_types, bool isDefined) :
        _type(type), _name(name), _qualifier(convertToInternal(noQualifier, isPublic, isUse)), _function(isFunction), _param_types(param_types), _defined(isDefined) {
    }

    virtual ~symbol() {
      delete _type;
    }

    basic_type *type() const {
      return _type;
    }
    const std::string &name() const {
      return _name;
    }
    inline bool noQualifier() {
      return _qualifier == QUALIFIER_NONE;
    }
    inline bool isPublic() {
      return _qualifier == QUALIFIER_PUBLIC;
    }
    inline bool isUse() {
      return _qualifier == QUALIFIER_USE;
    }
    inline bool isFunction() {
      return _function;
    }
    inline std::vector<basic_type*> param_types() {
      return _param_types;
    }
     inline basic_type* param_type_at(int i) {
      return _param_types.at(i);
    }
    inline bool isDefined() {
      return _defined;
    }
    inline bool setDefined(bool val) {
      return _defined = val;
    }
    inline bool setGlobal(bool val) {
      return _global=val;
    }
    inline bool getGlobal() {
      return _global;
    }

  private:
    inline int convertToInternal(bool noQualifier, bool isPublic, bool isUse) {
      return noQualifier*QUALIFIER_NONE + isPublic*QUALIFIER_PUBLIC + isUse*QUALIFIER_USE;
    }

  public:
    static const int QUALIFIER_NONE = 0;
    static const int QUALIFIER_PUBLIC = 1;
    static const int QUALIFIER_USE = 2;
  };

} // gr8

#endif
