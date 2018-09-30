#include "method.h"

using namespace std;

Method::Method(const QString &mname):visibility(PUBLIC),const_(false),virtual_(false),static_(false),abstract_(isAbstract())
{
    name = new QString(mname);
    returntype = new QString("void");
    args = unordered_map<QString*,QString*>();
}

Method::Method(const QString &mname, const QString &rtype, Visibility visi, bool isconst, bool isvirtual, bool isstatic, bool isabstract):visibility(visi),const_(isconst),virtual_(isvirtual),static_(isstatic),abstract_(isabstract)
{
    name = new QString(mname);
    returntype = new QString(rtype);
    args = unordered_map<QString*,QString*>();
}

Method::~Method()
{
    delete name;
    delete returntype;
    for (auto& i: args)
    {
        delete i.first;
        delete i.second;
    }
}

void Method::add_args(const QString &arg_name, const QString &arg_type)
{
    args.insert(make_pair<QString*,QString*>(new QString(arg_name),new QString(arg_type)));
}

void Method::rm_args(const QString &arg_name)
{
    for (auto& i:args)
    {
        if (arg_name.compare(i.first) == 0)
        {
            delete i.second;
            delete i.first;
            args.erase(i.first);
            break;
        }
    }
}

QString Method::getArgsAsQString()
{
    QString str("");
    for (auto& i:args)
    {
        str += *(i.second);
        str += " ";
        str += *(i.first);
        str += ", ";
    }
    if (!args.empty())
    {
        str.remove(str.size()-2,2);
    }
    return str;
}
