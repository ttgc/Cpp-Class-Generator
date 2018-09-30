#ifndef MEMBER_H
#define MEMBER_H

#include <QString>
#include "visibility.h"

class Member
{
    public:
        Member(QString const& mbname, QString const& mbtype);
        Member(QString const& mbname, QString const& mbtype, Visibility visi, bool get, bool set, bool isstatic);
        ~Member();
        QString* getName() const {return name;}
        QString* getType() const {return type;}
        Visibility getVisibility() const {return visibility;}
        bool hasGetter() const {return getter;}
        bool hasSetter() const {return setter;}
        bool isStatic() const {return static_;}

    protected:
        QString* name;
        QString* type;
        Visibility visibility;
        bool getter;
        bool setter;
        bool static_;
};

#endif // MEMBER_H
