#include "member.h"

Member::Member(QString const& mbname, QString const& mbtype):visibility(PRIVATE),getter(false),setter(false),static_(false)
{
    name = new QString(mbname);
    type = new QString(mbtype);
}

Member::Member(const QString &mbname, const QString &mbtype, Visibility visi, bool get, bool set, bool isstatic):visibility(visi),getter(get),setter(set),static_(isstatic)
{
    name = new QString(mbname);
    type = new QString(mbtype);
}

Member::~Member()
{
    delete name;
    delete type;
}
