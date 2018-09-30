#include "ancestor.h"

Ancestor::Ancestor(QString const& ancestor):visibility(PUBLIC)
{
    name = new QString(ancestor);
    file = new QString("<"+ancestor+".h>");
}

Ancestor::Ancestor(QString const& ancestor, Visibility visi, QString const& import):visibility(visi)
{
    name = new QString(ancestor);
    file = new QString(import);
}

Ancestor::~Ancestor()
{
    delete name;
    delete file;
}
