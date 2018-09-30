#ifndef METHOD_H
#define METHOD_H

#include <unordered_map>
#include <QString>
#include "visibility.h"

class Method
{
    public:
        Method(QString const& mname);
        Method(QString const& mname, QString const& rtype, Visibility visi, bool isconst, bool isvirtual, bool isstatic, bool isabstract);
        ~Method();
        void add_args(QString const& arg_name, QString const& arg_type);
        void rm_args(QString const& arg_name);
        QString getArgsAsQString();
        QString* getName() const {return name;}
        QString* getReturntype() const {return returntype;}
        Visibility getVisibility() const {return visibility;}
        std::unordered_map<QString*,QString*> getArgs() const {return args;}
        bool isConst() const {return const_;}
        bool isVirtual() const {return virtual_;}
        bool isStatic() const {return static_;}
        bool isAbstract() const {return abstract_;}
        void setName(QString const& new_) {name = new QString(new_);}
        void setReturntype(QString const& new_) {returntype = new QString(new_);}
        void setVisibility(Visibility new_) {visibility = new_;}
        void setConst(bool new_) {const_ = new_;}
        void setVirtual(bool new_) {virtual_ = new_;}
        void setStatic(bool new_) {static_ = new_;}
        void setAbstract(bool new_) {abstract_ = new_;}

    protected:
        QString* name;
        QString* returntype;
        Visibility visibility;
        std::unordered_map<QString*,QString*> args;
        bool const_;
        bool virtual_;
        bool static_;
        bool abstract_;
};

#endif // METHOD_H
