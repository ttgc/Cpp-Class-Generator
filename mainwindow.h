#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <unordered_map>
#include <QtWidgets>
#include "ancestor.h"
#include "member.h"
#include "method.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public slots:
        void updatenames(QString text);
        void add_ancestor();
        void modify_ancestor();
        void rm_ancestor();
        void update_ancestorimport(QString text);
        void update_ancestorfields(int index);
        void destructor_enable(bool state);
        void opadd_enable(bool state);
        void opsub_enable(bool state);
        void opmul_enable(bool state);
        void opdiv_enable(bool state);
        void opmod_enable(bool state);
        //void opincr_enable(bool state);
        //void opdecr_enable(bool state);
        void add_member();
        void modify_member();
        void rm_member();
        void update_memberfields(int index);
        void add_arg_method();
        void rm_arg_method();
        void add_method();
        void rm_method();
        void method_abstract_enable(bool state);
        void method_static_enabling(bool state);
        void generate();

    private:
        QWidget* central;
        QTabWidget* tabs;
        std::vector<QWidget*> pages;
        std::vector<QLayout*> layout;
        std::unordered_map<std::string,QWidget*> widgets;
        std::vector<Ancestor*> ancestors;
        std::vector<Member*> members;
        std::vector<Method*> methods;
        Method* current_method;
};

#endif // MAINWINDOW_H
