#include <vector>
#include <fstream>
#include "visibility.h"
#include "mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    tabs = new QTabWidget;
    pages = vector<QWidget*>();
    layout = vector<QLayout*>();
    widgets = unordered_map<string,QWidget*>();
    ancestors = vector<Ancestor*>();
    members = vector<Member*>();
    methods = vector<Method*>();
    current_method = new Method("cur");
    central = new QWidget;
    QWidget* page;

    QPushButton* generate = new QPushButton("&Generate Class");
    widgets.insert(make_pair<string,QWidget*>("end",generate));
    QVBoxLayout* mainlayout = new QVBoxLayout;
    mainlayout->addWidget(tabs);
    mainlayout->addWidget(generate);
    layout.push_back(mainlayout);//id=0
    central->setLayout(mainlayout);
    setWindowTitle("C++ Class Generator");
    setCentralWidget(central);
    connect(generate,SIGNAL(pressed()),this,SLOT(generate()));

    page = new QWidget;
    QLineEdit* clname = new QLineEdit;
    QLineEdit* clsrcfile = new QLineEdit;
    QLineEdit* clhfile = new QLineEdit;
    QLineEdit* cldefine = new QLineEdit;
    QFormLayout* genlayout = new QFormLayout;
    genlayout->addRow("Class name :",clname);
    genlayout->addRow("Source file :",clsrcfile);
    genlayout->addRow("Header file :",clhfile);
    genlayout->addRow("Header constant name :",cldefine);
    widgets.insert(make_pair<string,QWidget*>("name",clname));
    widgets.insert(make_pair<string,QWidget*>("src",clsrcfile));
    widgets.insert(make_pair<string,QWidget*>("header",clhfile));
    widgets.insert(make_pair<string,QWidget*>("define",cldefine));
    layout.push_back(genlayout); //id=1
    page->setLayout(genlayout);
    pages.push_back(page); //id=0
    tabs->addTab(page,"Main");
    clsrcfile->setText(".cpp");
    clhfile->setText(".h");
    cldefine->setText("_H");
    connect(clname,SIGNAL(textChanged(QString)),this,SLOT(updatenames(QString)));

    page = new QWidget;
    QLineEdit* clancestor = new QLineEdit;
    QLabel* clancestorlbl = new QLabel("Ancestor :");
    QGroupBox* clvisibility = new QGroupBox("Visibility :");
    QRadioButton* visipublic = new QRadioButton("Public");
    QRadioButton* visiprotected = new QRadioButton("Protected");
    QRadioButton* visiprivate = new QRadioButton("Private");
    visipublic->setChecked(true);
    QHBoxLayout* visilayout = new QHBoxLayout;
    visilayout->addWidget(visipublic);
    visilayout->addWidget(visiprotected);
    visilayout->addWidget(visiprivate);
    clvisibility->setLayout(visilayout);
    QLineEdit* climport = new QLineEdit;
    QLabel* climportlbl = new QLabel("Include file :");
    QListWidget* ancestorsls = new QListWidget;
    ancestorsls->setSelectionMode(QAbstractItemView::SingleSelection);
    ancestorsls->setMinimumHeight(500);
    QPushButton* ancestorsadd = new QPushButton("&Add");
    QPushButton* ancestorsrm = new QPushButton("&Remove");
    QPushButton* ancestorsmodify = new QPushButton("&Modify");
    QGridLayout* ancestors_layout = new QGridLayout;
    ancestors_layout->addWidget(clancestorlbl,0,0);
    ancestors_layout->addWidget(clancestor,0,1);
    ancestors_layout->addWidget(clvisibility,1,0,1,2);
    ancestors_layout->addWidget(climportlbl,2,0);
    ancestors_layout->addWidget(climport,2,1);
    ancestors_layout->addWidget(ancestorsls,0,2,3,1,Qt::AlignCenter);
    ancestors_layout->addWidget(ancestorsadd,3,0);
    ancestors_layout->addWidget(ancestorsmodify,3,1);
    ancestors_layout->addWidget(ancestorsrm,3,2);
    page->setLayout(ancestors_layout);
    tabs->addTab(page,"Ancestors");
    widgets.insert(make_pair<string,QWidget*>("ancestorlbl",clancestorlbl));
    widgets.insert(make_pair<string,QWidget*>("ancestor",clancestor));
    widgets.insert(make_pair<string,QWidget*>("ancestor_visibility",clvisibility));
    widgets.insert(make_pair<string,QWidget*>("ancestor_visi_public",visipublic));
    widgets.insert(make_pair<string,QWidget*>("ancestor_visi_protected",visiprotected));
    widgets.insert(make_pair<string,QWidget*>("ancestor_visi_private",visiprivate));
    layout.push_back(visilayout); //id=2
    widgets.insert(make_pair<string,QWidget*>("ancestor_importlbl",climportlbl));
    widgets.insert(make_pair<string,QWidget*>("ancestor_import",climport));
    widgets.insert(make_pair<string,QWidget*>("ancestor_list",ancestorsls));
    widgets.insert(make_pair<string,QWidget*>("ancestor_add",ancestorsadd));
    widgets.insert(make_pair<string,QWidget*>("ancestor_rm",ancestorsrm));
    widgets.insert(make_pair<string,QWidget*>("ancestor_modify",ancestorsmodify));
    layout.push_back(ancestors_layout); //id=3
    pages.push_back(page); //id=1
    connect(clancestor,SIGNAL(textChanged(QString)),this,SLOT(update_ancestorimport(QString)));
    connect(ancestorsadd,SIGNAL(pressed()),this,SLOT(add_ancestor()));
    connect(ancestorsrm,SIGNAL(pressed()),this,SLOT(rm_ancestor()));
    connect(ancestorsmodify,SIGNAL(pressed()),this,SLOT(modify_ancestor()));
    connect(ancestorsls,SIGNAL(currentRowChanged(int)),this,SLOT(update_ancestorfields(int)));

    page = new QWidget;
    QGroupBox* constrgrp = new QGroupBox("Constructor(s)");
    QLabel* constrparamlbl = new QLabel("Constructor parameters :");
    QLineEdit* constrparam = new QLineEdit;
    QCheckBox* constrcpy = new QCheckBox("Generate copy constructor ?");
    QHBoxLayout* constrlayout = new QHBoxLayout;
    constrlayout->addWidget(constrparamlbl);
    constrlayout->addWidget(constrparam);
    constrlayout->addWidget(constrcpy);
    constrgrp->setLayout(constrlayout);
    QGroupBox* destrgrp = new QGroupBox("Destructor");
    QCheckBox* destr = new QCheckBox("Has destructor ?");
    QCheckBox* virtdestr = new QCheckBox("Virtual destructor ?");
    virtdestr->setChecked(true);
    virtdestr->setDisabled(true);
    QHBoxLayout* destrlayout = new QHBoxLayout;
    destrlayout->addWidget(destr);
    destrlayout->addWidget(virtdestr);
    destrgrp->setLayout(destrlayout);
    QGroupBox* operatorgrp = new QGroupBox("Operators");
    QLabel* optype = new QLabel("Operator type");
    QLabel* opparam = new QLabel("Operator parameters type");
    QLabel* opreturn = new QLabel("Return type");
    QCheckBox* opadd = new QCheckBox("Operator +");
    QLineEdit* opaddtype = new QLineEdit;
    opaddtype->setDisabled(true);
    QLineEdit* opaddrtype = new QLineEdit;
    opaddrtype->setDisabled(true);
    QCheckBox* opsub = new QCheckBox("Operator -");
    QLineEdit* opsubtype = new QLineEdit;
    opsubtype->setDisabled(true);
    QLineEdit* opsubrtype = new QLineEdit;
    opsubrtype->setDisabled(true);
    QCheckBox* opmul = new QCheckBox("Operator *");
    QLineEdit* opmultype = new QLineEdit;
    opmultype->setDisabled(true);
    QLineEdit* opmulrtype = new QLineEdit;
    opmulrtype->setDisabled(true);
    QCheckBox* opdiv = new QCheckBox("Operator /");
    QLineEdit* opdivtype = new QLineEdit;
    opdivtype->setDisabled(true);
    QLineEdit* opdivrtype = new QLineEdit;
    opdivrtype->setDisabled(true);
    QCheckBox* opmod = new QCheckBox("Operator %");
    QLineEdit* opmodtype = new QLineEdit;
    opmodtype->setDisabled(true);
    QLineEdit* opmodrtype = new QLineEdit;
    opmodrtype->setDisabled(true);
    QCheckBox* opincr = new QCheckBox("Operator ++");
    //QLineEdit* opincrrtype = new QLineEdit;
    //opincrrtype->setDisabled(true);
    QCheckBox* opdecr = new QCheckBox("Operator --");
    //QLineEdit* opdecrrtype = new QLineEdit;
    //opdecrrtype->setDisabled(true);
    QLabel* opother = new QLabel("Other common operators :");
    opother->setAlignment(Qt::AlignCenter);
    QCheckBox* opeq = new QCheckBox("Operator =");
    QCheckBox* opneq = new QCheckBox("Operator !=");
    QCheckBox* oplower = new QCheckBox("Operator <");
    QCheckBox* oplowereq = new QCheckBox("Operator <=");
    QCheckBox* opsup = new QCheckBox("Operator >");
    QCheckBox* opsupeq = new QCheckBox("Operator >=");
    QCheckBox* opassign = new QCheckBox("Assignement operator");
    QGridLayout* oplayout = new QGridLayout;
    oplayout->addWidget(optype,0,0);
    oplayout->addWidget(opparam,0,1);
    oplayout->addWidget(opreturn,0,2);
    oplayout->addWidget(opadd,1,0);
    oplayout->addWidget(opaddtype,1,1);
    oplayout->addWidget(opaddrtype,1,2);
    oplayout->addWidget(opsub,2,0);
    oplayout->addWidget(opsubtype,2,1);
    oplayout->addWidget(opsubrtype,2,2);
    oplayout->addWidget(opmul,3,0);
    oplayout->addWidget(opmultype,3,1);
    oplayout->addWidget(opmulrtype,3,2);
    oplayout->addWidget(opdiv,4,0);
    oplayout->addWidget(opdivtype,4,1);
    oplayout->addWidget(opdivrtype,4,2);
    oplayout->addWidget(opmod,5,0);
    oplayout->addWidget(opmodtype,5,1);
    oplayout->addWidget(opmodrtype,5,2);
    oplayout->addWidget(opincr,6,0);
    //oplayout->addWidget(opincrrtype,6,2);
    oplayout->addWidget(opdecr,7,0);
    //oplayout->addWidget(opdecrrtype,7,2);
    oplayout->addWidget(opother,8,0,1,3);
    oplayout->addWidget(opeq,9,0);
    oplayout->addWidget(opneq,9,1);
    oplayout->addWidget(oplower,9,2);
    oplayout->addWidget(oplowereq,10,0);
    oplayout->addWidget(opsup,10,1);
    oplayout->addWidget(opsupeq,10,2);
    oplayout->addWidget(opassign,11,0,1,3);
    operatorgrp->setLayout(oplayout);
    QVBoxLayout* settinglayout = new QVBoxLayout;
    settinglayout->addWidget(constrgrp);
    settinglayout->addWidget(destrgrp);
    settinglayout->addWidget(operatorgrp);
    settinglayout->setAlignment(Qt::AlignCenter);
    page->setLayout(settinglayout);
    tabs->addTab(page,"Settings");
    widgets.insert(make_pair<string,QWidget*>("constructor_group",constrgrp));
    widgets.insert(make_pair<string,QWidget*>("constructor_label",constrparamlbl));
    widgets.insert(make_pair<string,QWidget*>("constructor_parameters",constrparam));
    widgets.insert(make_pair<string,QWidget*>("constructor_copy",constrcpy));
    layout.push_back(constrlayout);//id=4
    widgets.insert(make_pair<string,QWidget*>("destructor_group",destrgrp));
    widgets.insert(make_pair<string,QWidget*>("destructor",destr));
    widgets.insert(make_pair<string,QWidget*>("destructor_virtual",virtdestr));
    layout.push_back(destrlayout);//id=5
    widgets.insert(make_pair<string,QWidget*>("operator_group",operatorgrp));
    widgets.insert(make_pair<string,QWidget*>("oplbl",optype));
    widgets.insert(make_pair<string,QWidget*>("oplbl_param",opparam));
    widgets.insert(make_pair<string,QWidget*>("oplbl_return",opreturn));
    widgets.insert(make_pair<string,QWidget*>("operator+",opadd));
    widgets.insert(make_pair<string,QWidget*>("operator-",opsub));
    widgets.insert(make_pair<string,QWidget*>("operator*",opmul));
    widgets.insert(make_pair<string,QWidget*>("operator/",opdiv));
    widgets.insert(make_pair<string,QWidget*>("operator%",opmod));
    widgets.insert(make_pair<string,QWidget*>("operator++",opincr));
    widgets.insert(make_pair<string,QWidget*>("operator--",opdecr));
    widgets.insert(make_pair<string,QWidget*>("op+param",opaddtype));
    widgets.insert(make_pair<string,QWidget*>("op-param",opsubtype));
    widgets.insert(make_pair<string,QWidget*>("op*param",opmultype));
    widgets.insert(make_pair<string,QWidget*>("op/param",opdivtype));
    widgets.insert(make_pair<string,QWidget*>("op%param",opmodtype));
    widgets.insert(make_pair<string,QWidget*>("op+return",opaddrtype));
    widgets.insert(make_pair<string,QWidget*>("op-return",opsubrtype));
    widgets.insert(make_pair<string,QWidget*>("op*return",opmulrtype));
    widgets.insert(make_pair<string,QWidget*>("op/return",opdivrtype));
    widgets.insert(make_pair<string,QWidget*>("op%return",opmodrtype));
    //widgets.insert(make_pair<string,QWidget*>("op++return",opincrrtype));
    //widgets.insert(make_pair<string,QWidget*>("op--return",opdecrrtype));
    widgets.insert(make_pair<string,QWidget*>("oplbl_other",opother));
    widgets.insert(make_pair<string,QWidget*>("operator==",opeq));
    widgets.insert(make_pair<string,QWidget*>("operator!=",opneq));
    widgets.insert(make_pair<string,QWidget*>("operator<",oplower));
    widgets.insert(make_pair<string,QWidget*>("operator<=",oplowereq));
    widgets.insert(make_pair<string,QWidget*>("operator>",opsup));
    widgets.insert(make_pair<string,QWidget*>("operator>=",opsupeq));
    widgets.insert(make_pair<string,QWidget*>("operator=",opassign));
    layout.push_back(oplayout);//id=6
    layout.push_back(settinglayout);//id=7
    pages.push_back(page);//id=2
    connect(destr,SIGNAL(toggled(bool)),this,SLOT(destructor_enable(bool)));
    connect(opadd,SIGNAL(toggled(bool)),this,SLOT(opadd_enable(bool)));
    connect(opsub,SIGNAL(toggled(bool)),this,SLOT(opsub_enable(bool)));
    connect(opmul,SIGNAL(toggled(bool)),this,SLOT(opmul_enable(bool)));
    connect(opdiv,SIGNAL(toggled(bool)),this,SLOT(opdiv_enable(bool)));
    connect(opmod,SIGNAL(toggled(bool)),this,SLOT(opmod_enable(bool)));
    //connect(opincr,SIGNAL(toggled(bool)),this,SLOT(opincr_enable(bool)));
    //connect(opdecr,SIGNAL(toggled(bool)),this,SLOT(opdecr_enable(bool)));

    page = new QWidget;
    QLabel* mbnamelbl = new QLabel("Name :");
    QLabel* mbtypelbl = new QLabel("Type :");
    QLineEdit* mbname = new QLineEdit("m_");
    QLineEdit* mbtype = new QLineEdit;
    QGroupBox* mbvisibility = new QGroupBox("Visibility :");
    QRadioButton* mbvisipublic = new QRadioButton("Public");
    QRadioButton* mbvisiprotected = new QRadioButton("Protected");
    QRadioButton* mbvisiprivate = new QRadioButton("Private");
    mbvisiprivate->setChecked(true);
    QHBoxLayout* mbvisilayout = new QHBoxLayout;
    mbvisilayout->addWidget(mbvisipublic);
    mbvisilayout->addWidget(mbvisiprotected);
    mbvisilayout->addWidget(mbvisiprivate);
    mbvisibility->setLayout(mbvisilayout);
    QCheckBox* mbgetter = new QCheckBox("Has getter ?");
    QCheckBox* mbsetter = new QCheckBox("Has setter ?");
    QCheckBox* mbstatic = new QCheckBox("Is static ?");
    QListWidget* mbls = new QListWidget;
    mbls->setSelectionMode(QAbstractItemView::SingleSelection);
    mbls->setMinimumHeight(500);
    QPushButton* mbadd = new QPushButton("&Add");
    QPushButton* mbrm = new QPushButton("&Remove");
    QPushButton* mbmodify = new QPushButton("&Modify");
    QGridLayout* mblayout = new QGridLayout;
    mblayout->addWidget(mbnamelbl,0,0);
    mblayout->addWidget(mbtypelbl,1,0);
    mblayout->addWidget(mbname,0,1);
    mblayout->addWidget(mbtype,1,1);
    mblayout->addWidget(mbvisibility,2,0,1,2);
    mblayout->addWidget(mbgetter,3,0);
    mblayout->addWidget(mbsetter,3,1);
    mblayout->addWidget(mbstatic,4,0,1,2);
    mblayout->addWidget(mbls,0,2,5,1,Qt::AlignCenter);
    mblayout->addWidget(mbadd,5,0);
    mblayout->addWidget(mbmodify,5,1);
    mblayout->addWidget(mbrm,5,2);
    page->setLayout(mblayout);
    tabs->addTab(page,"Members");
    widgets.insert(make_pair<string,QWidget*>("mbnamelbl",mbnamelbl));
    widgets.insert(make_pair<string,QWidget*>("mbtypelbl",mbtypelbl));
    widgets.insert(make_pair<string,QWidget*>("mbname",mbname));
    widgets.insert(make_pair<string,QWidget*>("mbtype",mbtype));
    widgets.insert(make_pair<string,QWidget*>("mbvisibility",mbvisibility));
    widgets.insert(make_pair<string,QWidget*>("mbvisipublic",mbvisipublic));
    widgets.insert(make_pair<string,QWidget*>("mbvisiprotected",mbvisiprotected));
    widgets.insert(make_pair<string,QWidget*>("mbvisiprivate",mbvisiprivate));
    layout.push_back(mbvisilayout);//id=8
    widgets.insert(make_pair<string,QWidget*>("mbgetter",mbgetter));
    widgets.insert(make_pair<string,QWidget*>("mbsetter",mbsetter));
    widgets.insert(make_pair<string,QWidget*>("mbstatic",mbstatic));
    widgets.insert(make_pair<string,QWidget*>("mbls",mbls));
    widgets.insert(make_pair<string,QWidget*>("mbadd",mbadd));
    widgets.insert(make_pair<string,QWidget*>("mbmodify",mbmodify));
    widgets.insert(make_pair<string,QWidget*>("mbrm",mbrm));
    layout.push_back(mblayout);//id=9
    pages.push_back(page);//id=3
    connect(mbadd,SIGNAL(pressed()),this,SLOT(add_member()));
    connect(mbrm,SIGNAL(pressed()),this,SLOT(rm_member()));
    connect(mbmodify,SIGNAL(pressed()),this,SLOT(modify_member()));
    connect(mbls,SIGNAL(currentRowChanged(int)),this,SLOT(update_memberfields(int)));

    page = new QWidget;
    QLabel* mhnamelbl = new QLabel("Name :");
    QLabel* mhtypelbl = new QLabel("Return type :");
    QLineEdit* mhname = new QLineEdit;
    QLineEdit* mhtype = new QLineEdit;
    QGroupBox* mhvisibility = new QGroupBox("Visibility :");
    QRadioButton* mhvisipublic = new QRadioButton("Public");
    QRadioButton* mhvisiprotected = new QRadioButton("Protected");
    QRadioButton* mhvisiprivate = new QRadioButton("Private");
    mhvisipublic->setChecked(true);
    QHBoxLayout* mhvisilayout = new QHBoxLayout;
    mhvisilayout->addWidget(mhvisipublic);
    mhvisilayout->addWidget(mhvisiprotected);
    mhvisilayout->addWidget(mhvisiprivate);
    mhvisibility->setLayout(mhvisilayout);
    QCheckBox* mhconst = new QCheckBox("Is constant ?");
    QCheckBox* mhvirtual = new QCheckBox("Is virtual ?");
    QCheckBox* mhstatic = new QCheckBox("Is static ?");
    QCheckBox* mhabstract = new QCheckBox("Is abstract ?");
    mhabstract->setDisabled(true);
    QLabel* mharrow = new QLabel(">>");
    QListWidget* mhargs = new QListWidget;
    mhargs->setSelectionMode(QAbstractItemView::SingleSelection);
    mhargs->setMinimumHeight(450);
    QGroupBox* mharggrp = new QGroupBox("Argument settings");
    QLineEdit* mhargtype = new QLineEdit;
    QLineEdit* mhargname = new QLineEdit;
    QFormLayout* mharglayout = new QFormLayout;
    mharglayout->addRow("Argument type :",mhargtype);
    mharglayout->addRow("Argument name :",mhargname);
    mharggrp->setLayout(mharglayout);
    QPushButton* mhargadd = new QPushButton("Add Argument");
    QPushButton* mhargrm = new QPushButton("Remove Argument");
    QListWidget* mhls = new QListWidget;
    mhls->setSelectionMode(QAbstractItemView::SingleSelection);
    mhls->setMinimumHeight(450);
    QPushButton* mhadd = new QPushButton("&Add Method");
    QPushButton* mhrm = new QPushButton("&Remove Method");
    QGridLayout* mhlayout = new QGridLayout;
    mhlayout->addWidget(mhnamelbl,0,0);
    mhlayout->addWidget(mhtypelbl,1,0);
    mhlayout->addWidget(mhname,0,1);
    mhlayout->addWidget(mhtype,1,1);
    mhlayout->addWidget(mhvisibility,2,0,1,2);
    mhlayout->addWidget(mhconst,3,1);
    mhlayout->addWidget(mhvirtual,4,0);
    mhlayout->addWidget(mhstatic,3,0);
    mhlayout->addWidget(mhabstract,4,1);
    mhlayout->addWidget(mharrow,0,3,7,1,Qt::AlignCenter);
    mhlayout->addWidget(mhargs,0,2,5,1,Qt::AlignCenter);
    mhlayout->addWidget(mharggrp,5,0,2,2);
    mhlayout->addWidget(mhargadd,5,2);
    mhlayout->addWidget(mhargrm,6,2);
    mhlayout->addWidget(mhls,0,4,5,1,Qt::AlignCenter);
    mhlayout->addWidget(mhadd,5,4);
    mhlayout->addWidget(mhrm,6,4);
    page->setLayout(mhlayout);
    tabs->addTab(page,"Methods");
    widgets.insert(make_pair<string,QWidget*>("mhnamelbl",mhnamelbl));
    widgets.insert(make_pair<string,QWidget*>("mhtypelbl",mhtypelbl));
    widgets.insert(make_pair<string,QWidget*>("mhname",mhname));
    widgets.insert(make_pair<string,QWidget*>("mhrtype",mhtype));
    widgets.insert(make_pair<string,QWidget*>("mhvisibility",mhvisibility));
    widgets.insert(make_pair<string,QWidget*>("mhvisipublic",mhvisipublic));
    widgets.insert(make_pair<string,QWidget*>("mhvisiprotected",mhvisiprotected));
    widgets.insert(make_pair<string,QWidget*>("mhvisiprivate",mhvisiprivate));
    layout.push_back(mhvisilayout);//id=10
    widgets.insert(make_pair<string,QWidget*>("mhconst",mhconst));
    widgets.insert(make_pair<string,QWidget*>("mhvirtual",mhvirtual));
    widgets.insert(make_pair<string,QWidget*>("mhstatic",mhstatic));
    widgets.insert(make_pair<string,QWidget*>("mhabstract",mhabstract));
    widgets.insert(make_pair<string,QWidget*>("mharrow",mharrow));
    widgets.insert(make_pair<string,QWidget*>("mhargs",mhargs));
    widgets.insert(make_pair<string,QWidget*>("mharggrp",mharggrp));
    widgets.insert(make_pair<string,QWidget*>("mhargtype",mhargtype));
    widgets.insert(make_pair<string,QWidget*>("mhargname",mhargname));
    layout.push_back(mharglayout);//id=11
    widgets.insert(make_pair<string,QWidget*>("mhargadd",mhargadd));
    widgets.insert(make_pair<string,QWidget*>("mhargrm",mhargrm));
    widgets.insert(make_pair<string,QWidget*>("mhls",mhls));
    widgets.insert(make_pair<string,QWidget*>("mhadd",mhadd));
    widgets.insert(make_pair<string,QWidget*>("mhrm",mhrm));
    layout.push_back(mhlayout);//id=12
    pages.push_back(page);//id=4
    connect(mhstatic,SIGNAL(toggled(bool)),this,SLOT(method_static_enabling(bool)));
    connect(mhvirtual,SIGNAL(toggled(bool)),this,SLOT(method_abstract_enable(bool)));
    connect(mhargadd,SIGNAL(pressed()),this,SLOT(add_arg_method()));
    connect(mhargrm,SIGNAL(pressed()),this,SLOT(rm_arg_method()));
    connect(mhadd,SIGNAL(pressed()),this,SLOT(add_method()));
    connect(mhrm,SIGNAL(pressed()),this,SLOT(rm_method()));


}

MainWindow::~MainWindow()
{
    for (unsigned int i(0);i<ancestors.size();i++)
    {
        delete ancestors[i];
        ancestors[i] = NULL;
    }
    ancestors.clear();
    for (unsigned int i(0);i<members.size();i++)
    {
        delete members[i];
        members[i] = NULL;
    }
    members.clear();
    for (unsigned int i(0);i<methods.size();i++)
    {
        delete methods[i];
        methods[i] = NULL;
    }
    methods.clear();
    delete current_method;
}

void MainWindow::updatenames(QString text)
{
    ((QLineEdit*)widgets["src"])->setText(text.toLower()+".cpp");
    ((QLineEdit*)widgets["header"])->setText(text.toLower()+".h");
    ((QLineEdit*)widgets["define"])->setText(text.toUpper()+"_H");
}

void MainWindow::add_ancestor()
{
    Visibility visi;
    QString visi_string;
    if (((QRadioButton*)widgets["ancestor_visi_protected"])->isChecked())
    {
        visi = PROTECTED;
        visi_string = "protected";
    }
    else if (((QRadioButton*)widgets["ancestor_visi_private"])->isChecked())
    {
        visi = PRIVATE;
        visi_string = "private";
    }
    else
    {
        visi = PUBLIC;
        visi_string = "public";
    }
    ancestors.push_back(new Ancestor(((QLineEdit*)widgets["ancestor"])->text(),visi,((QLineEdit*)widgets["ancestor_import"])->text()));
    ((QListWidget*)widgets["ancestor_list"])->addItem(((QLineEdit*)widgets["ancestor"])->text()+" : "+visi_string);
    ((QLineEdit*)widgets["ancestor"])->setText("");
    ((QLineEdit*)widgets["ancestor_import"])->setText("");
    ((QRadioButton*)widgets["ancestor_visi_public"])->setChecked(true);
}

void MainWindow::modify_ancestor()
{
    add_ancestor();
    rm_ancestor();
}

void MainWindow::rm_ancestor()
{
    int index(((QListWidget*)widgets["ancestor_list"])->currentRow());
    if (index >= 0)
    {
        delete ancestors[index];
        ancestors[index] = NULL;
        ancestors.erase(ancestors.begin()+index);
        ((QListWidget*)widgets["ancestor_list"])->takeItem(index);
    }
}

void MainWindow::update_ancestorimport(QString text)
{
    ((QLineEdit*)widgets["ancestor_import"])->setText("<"+text+">");
}

void MainWindow::update_ancestorfields(int index)
{
    if (index >= 0)
    {
        ((QLineEdit*)widgets["ancestor"])->setText(*(ancestors[index]->getName()));
        ((QLineEdit*)widgets["ancestor_import"])->setText(*(ancestors[index]->getFile()));
        switch(ancestors[index]->getVisibility())
        {
        case PUBLIC:
            ((QRadioButton*)widgets["ancestor_visi_public"])->setChecked(true);
            break;
        case PROTECTED:
            ((QRadioButton*)widgets["ancestor_visi_protected"])->setChecked(true);
            break;
        case PRIVATE:
            ((QRadioButton*)widgets["ancestor_visi_private"])->setChecked(true);
            break;
        }
    }
}

void MainWindow::destructor_enable(bool state)
{
    ((QCheckBox*)widgets["destructor_virtual"])->setEnabled(state);
}

void MainWindow::opadd_enable(bool state)
{
    ((QLineEdit*)widgets["op+param"])->setEnabled(state);
    ((QLineEdit*)widgets["op+return"])->setEnabled(state);
}

void MainWindow::opsub_enable(bool state)
{
    ((QLineEdit*)widgets["op-param"])->setEnabled(state);
    ((QLineEdit*)widgets["op-return"])->setEnabled(state);
}

void MainWindow::opmul_enable(bool state)
{
    ((QLineEdit*)widgets["op*param"])->setEnabled(state);
    ((QLineEdit*)widgets["op*return"])->setEnabled(state);
}

void MainWindow::opdiv_enable(bool state)
{
    ((QLineEdit*)widgets["op/param"])->setEnabled(state);
    ((QLineEdit*)widgets["op/return"])->setEnabled(state);
}

void MainWindow::opmod_enable(bool state)
{
    ((QLineEdit*)widgets["op%param"])->setEnabled(state);
    ((QLineEdit*)widgets["op%return"])->setEnabled(state);
}

//void MainWindow::opincr_enable(bool state)
//{
//    ((QLineEdit*)widgets["op++return"])->setEnabled(state);
//}

//void MainWindow::opdecr_enable(bool state)
//{
//    ((QLineEdit*)widgets["op--return"])->setEnabled(state);
//}

void MainWindow::add_member()
{
    Visibility visi;
    QString visi_string;
    if (((QRadioButton*)widgets["mbvisiprotected"])->isChecked())
    {
        visi = PROTECTED;
        visi_string = "protected";
    }
    else if (((QRadioButton*)widgets["mbvisipublic"])->isChecked())
    {
        visi = PUBLIC;
        visi_string = "public";
    }
    else
    {
        visi = PRIVATE;
        visi_string = "private";
    }
    members.push_back(new Member(((QLineEdit*)widgets["mbname"])->text(),((QLineEdit*)widgets["mbtype"])->text(),visi,((QCheckBox*)widgets["mbgetter"])->isChecked(),((QCheckBox*)widgets["mbsetter"])->isChecked(),((QCheckBox*)widgets["mbstatic"])->isChecked()));
    ((QListWidget*)widgets["mbls"])->addItem(visi_string+" "+((QLineEdit*)widgets["mbtype"])->text()+" "+((QLineEdit*)widgets["mbname"])->text());
    ((QLineEdit*)widgets["mbname"])->setText("m_");
    ((QLineEdit*)widgets["mbtype"])->setText("");
    ((QRadioButton*)widgets["mbvisiprivate"])->setChecked(true);
}

void MainWindow::modify_member()
{
    add_member();
    rm_member();
}

void MainWindow::rm_member()
{
    int index(((QListWidget*)widgets["mbls"])->currentRow());
    if (index >= 0)
    {
        delete members[index];
        members[index] = NULL;
        members.erase(members.begin()+index);
        ((QListWidget*)widgets["mbls"])->takeItem(index);
    }
}

void MainWindow::update_memberfields(int index)
{
    if (index >= 0)
    {
        ((QLineEdit*)widgets["mbname"])->setText(*(members[index]->getName()));
        ((QLineEdit*)widgets["mbtype"])->setText(*(members[index]->getType()));
        switch(members[index]->getVisibility())
        {
        case PUBLIC:
            ((QRadioButton*)widgets["mbvisipublic"])->setChecked(true);
            break;
        case PROTECTED:
            ((QRadioButton*)widgets["mbvisiprotected"])->setChecked(true);
            break;
        case PRIVATE:
            ((QRadioButton*)widgets["mbvisiprivate"])->setChecked(true);
            break;
        }
        ((QCheckBox*)widgets["mbgetter"])->setChecked(members[index]->hasGetter());
        ((QCheckBox*)widgets["mbsetter"])->setChecked(members[index]->hasSetter());
        ((QCheckBox*)widgets["mbstatic"])->setChecked(members[index]->isStatic());
    }
}

void MainWindow::add_arg_method()
{
    current_method->add_args(((QLineEdit*)widgets["mhargname"])->text(),((QLineEdit*)widgets["mhargtype"])->text());
    ((QListWidget*)widgets["mhargs"])->addItem(((QLineEdit*)widgets["mhargname"])->text()+" : "+((QLineEdit*)widgets["mhargtype"])->text());
    ((QLineEdit*)widgets["mhargname"])->setText("");
    ((QLineEdit*)widgets["mhargtype"])->setText("");
}

void MainWindow::rm_arg_method()
{
    int index(((QListWidget*)widgets["mhargs"])->currentRow());
    if (index >= 0)
    {
        current_method->rm_args(((QListWidget*)widgets["mhargs"])->item(index)->text().split(" : ")[0]);
        ((QListWidget*)widgets["mhargs"])->takeItem(index);
    }
}

void MainWindow::add_method()
{
    Visibility visi;
    QString visi_string;
    if (((QRadioButton*)widgets["mhvisiprotected"])->isChecked())
    {
        visi = PROTECTED;
        visi_string = "protected";
    }
    else if (((QRadioButton*)widgets["mhvisiprivate"])->isChecked())
    {
        visi = PRIVATE;
        visi_string = "private";
    }
    else
    {
        visi = PUBLIC;
        visi_string = "public";
    }
    current_method->setName(((QLineEdit*)widgets["mhname"])->text());
    current_method->setReturntype(((QLineEdit*)widgets["mhrtype"])->text());
    current_method->setVisibility(visi);
    current_method->setAbstract(((QCheckBox*)widgets["mhabstract"])->isChecked());
    current_method->setConst(((QCheckBox*)widgets["mhconst"])->isChecked());
    current_method->setStatic(((QCheckBox*)widgets["mhstatic"])->isChecked());
    current_method->setVirtual(((QCheckBox*)widgets["mhvirtual"])->isChecked());
    methods.push_back(current_method);
    current_method = new Method("cur");
    ((QListWidget*)widgets["mhls"])->addItem(visi_string+" "+((QLineEdit*)widgets["mhrtype"])->text()+" "+((QLineEdit*)widgets["mhname"])->text()+"(...)");
    ((QLineEdit*)widgets["mhname"])->setText("");
    ((QLineEdit*)widgets["mhrtype"])->setText("");
    ((QRadioButton*)widgets["mhvisipublic"])->setChecked(true);
    ((QListWidget*)widgets["mhargs"])->clear();
}

void MainWindow::rm_method()
{
    int index(((QListWidget*)widgets["mhls"])->currentRow());
    if (index >= 0)
    {
        delete methods[index];
        methods[index] = NULL;
        methods.erase(methods.begin()+index);
        ((QListWidget*)widgets["mhls"])->takeItem(index);
    }
}

void MainWindow::method_abstract_enable(bool state)
{
    ((QCheckBox*)widgets["mhabstract"])->setEnabled(state && !((QCheckBox*)widgets["mhstatic"])->isChecked());
}

void MainWindow::method_static_enabling(bool state)
{
    ((QCheckBox*)widgets["mhabstract"])->setEnabled(!state && ((QCheckBox*)widgets["mhvirtual"])->isChecked());
    ((QCheckBox*)widgets["mhconst"])->setEnabled(!state);
    ((QCheckBox*)widgets["mhvirtual"])->setEnabled(!state);
}

void MainWindow::generate()
{
    QString directory = QFileDialog::getExistingDirectory(this);
    QString headerpath = directory+"/"+((QLineEdit*)widgets["header"])->text();
    QString srcpath = directory+"/"+((QLineEdit*)widgets["src"])->text();
    ofstream header(headerpath.toStdString());
    ofstream src(srcpath.toStdString());
    if (header && src)
    {
        header << "// Generated with Cpp Class Generator made by Ttgc" << endl;
        src << "// Generated with Cpp Class Generator made by Ttgc" << endl;
        header << "#ifndef " << ((QLineEdit*)widgets["define"])->text().toStdString() << endl;
        header << "#define " << ((QLineEdit*)widgets["define"])->text().toStdString() << endl << endl;
        src << "#include \"" << ((QLineEdit*)widgets["header"])->text().toStdString() << "\"" << endl << endl;
        for (unsigned int i(0);i<members.size();i++)
        {
            if (members[i]->isStatic())
            {
                src << members[i]->getType()->toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::" << members[i]->getName()->toStdString() << ";" << endl;
            }
        }
        src << endl;
        for (unsigned int i(0);i<ancestors.size();i++)
        {
            header << "#include " << ancestors[i]->getFile()->toStdString() << endl;
        }
        header << endl;
        header << "class " << ((QLineEdit*)widgets["name"])->text().toStdString();
        if (!ancestors.empty())
        {
            header << ":";
            for (unsigned int i(0);i<ancestors.size()-1;i++)
            {
                header << " " << translate_visibility(ancestors[i]->getVisibility()) << " " << ancestors[i]->getName()->toStdString() << ",";
            }
            header << " " << translate_visibility(ancestors[ancestors.size()-1]->getVisibility()) << " " << ancestors[ancestors.size()-1]->getName()->toStdString();
        }
        header << endl;
        header << "{" << endl;
        header << "\t" << "public:" <<endl;
        header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << "(" << ((QLineEdit*)widgets["constructor_parameters"])->text().toStdString() << ");" << endl;
        src << ((QLineEdit*)widgets["name"])->text().toStdString() << "::" << ((QLineEdit*)widgets["name"])->text().toStdString() << "(" << ((QLineEdit*)widgets["constructor_parameters"])->text().toStdString() << ")" << endl;
        src << "{" << endl;
        src << "\t" << "// constructor" << endl;
        src << "}" << endl << endl;
        if (((QCheckBox*)widgets["constructor_copy"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << "(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other);" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << "::" << ((QLineEdit*)widgets["name"])->text().toStdString() << "(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other)" << endl;
            src << "{" << endl;
            src << "\t" << "// copy constructor" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["destructor"])->isChecked())
        {
            header << "\t\t";
            if (((QCheckBox*)widgets["destructor_virtual"])->isChecked())
            {
                header << "virtual ";
            }
            header << "~" << ((QLineEdit*)widgets["name"])->text().toStdString() << "();" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << "::~" << ((QLineEdit*)widgets["name"])->text().toStdString() << "()" << endl;
            src << "{" << endl;
            src << "\t" << "// destructor" << endl;
            src << "}" << endl << endl;
        }
        header << endl;
        if (((QCheckBox*)widgets["operator="])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << "& operator=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other);" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << "& " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other)" << endl;
            src << "{" << endl;
            src << "\t" << "if (this == &other) return *this;" << endl;
            src << "\t" << "// assignment operator" << endl;
            src << "\t" << "return *this;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator+"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["op+return"])->text().toStdString() << " operator+(" << ((QLineEdit*)widgets["op+param"])->text().toStdString() << " const& other) const;" << endl;
            src << ((QLineEdit*)widgets["op+return"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator+(" << ((QLineEdit*)widgets["op+param"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator +" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator-"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["op-return"])->text().toStdString() << " operator-(" << ((QLineEdit*)widgets["op-param"])->text().toStdString() << " const& other) const;" << endl;
            src << ((QLineEdit*)widgets["op-return"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator-(" << ((QLineEdit*)widgets["op-param"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator -" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator*"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["op*return"])->text().toStdString() << " operator*(" << ((QLineEdit*)widgets["op*param"])->text().toStdString() << " const& other) const;" << endl;
            src << ((QLineEdit*)widgets["op*return"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator*(" << ((QLineEdit*)widgets["op*param"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator *" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator/"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["op/return"])->text().toStdString() << " operator/(" << ((QLineEdit*)widgets["op/param"])->text().toStdString() << " const& other) const;" << endl;
            src << ((QLineEdit*)widgets["op/return"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator/(" << ((QLineEdit*)widgets["op/param"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator /" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator%"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["op%return"])->text().toStdString() << " operator%(" << ((QLineEdit*)widgets["op%param"])->text().toStdString() << " const& other) const;" << endl;
            src << ((QLineEdit*)widgets["op%return"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator%(" << ((QLineEdit*)widgets["op%param"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator %" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator++"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << "& operator++();" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << "& " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator++()" << endl;
            src << "{" << endl;
            src << "\t" << "// operator prefix ++" << endl;
            src << "\t" << "return *this" << endl;
            src << "}" << endl << endl;
            header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << " operator++(int param);" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator++(int param)" << endl;
            src << "{" << endl;
            src << "\t" << "// operator postfix ++" << endl;
            src << "\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << " result(*this);" << endl;
            src << "\t" << "++(*this);" << endl;
            src << "\t" << "return result;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator--"])->isChecked())
        {
            header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << "& operator--();" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << "& " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator--()" << endl;
            src << "{" << endl;
            src << "\t" << "// operator prefix --" << endl;
            src << "\t" << "return *this" << endl;
            src << "}" << endl << endl;
            header << "\t\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << " operator--(int param);" << endl;
            src << ((QLineEdit*)widgets["name"])->text().toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator--(int param)" << endl;
            src << "{" << endl;
            src << "\t" << "// operator postfix --" << endl;
            src << "\t" << ((QLineEdit*)widgets["name"])->text().toStdString() << " result(*this);" << endl;
            src << "\t" << "--(*this);" << endl;
            src << "\t" << "return result;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator=="])->isChecked())
        {
            header << "\t\t" << "bool operator==(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const;" << endl;
            src << "bool " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator==(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator ==" << endl;
            src << "\t" << "return false;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator!="])->isChecked())
        {
            header << "\t\t" << "bool operator!=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const;" << endl;
            src << "bool " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator!=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator !=" << endl;
            src << "\t" << "return false;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator>"])->isChecked())
        {
            header << "\t\t" << "bool operator>(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const;" << endl;
            src << "bool " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator>(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator >" << endl;
            src << "\t" << "return false;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator<"])->isChecked())
        {
            header << "\t\t" << "bool operator<(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const;" << endl;
            src << "bool " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator<(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator <" << endl;
            src << "\t" << "return false;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator>="])->isChecked())
        {
            header << "\t\t" << "bool operator>=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const;" << endl;
            src << "bool " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator>=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator >=" << endl;
            src << "\t" << "return false;" << endl;
            src << "}" << endl << endl;
        }
        if (((QCheckBox*)widgets["operator<="])->isChecked())
        {
            header << "\t\t" << "bool operator<=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const;" << endl;
            src << "bool " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::operator<=(" << ((QLineEdit*)widgets["name"])->text().toStdString() << " const& other) const" << endl;
            src << "{" << endl;
            src << "\t" << "// operator <=" << endl;
            src << "\t" << "return false;" << endl;
            src << "}" << endl << endl;
        }
        header << endl;
        bool method_empty(true), member_empty(true);
        for (unsigned int i(0);i<methods.size();i++)
        {
            if (methods[i]->getVisibility() == PUBLIC)
            {
                method_empty = false;
                header << "\t\t";
                if (methods[i]->isStatic())
                {
                    header << "static ";
                }
                else if (methods[i]->isVirtual())
                {
                    header << "virtual ";
                }
                header << methods[i]->getReturntype()->toStdString() << " " << methods[i]->getName()->toStdString() << "(" << methods[i]->getArgsAsQString().toStdString() << ")";
                if (methods[i]->isConst() && !methods[i]->isStatic())
                {
                    header << " const";
                }
                if (methods[i]->isVirtual() && methods[i]->isAbstract() && !methods[i]->isStatic())
                {
                    header << " = 0";
                }
                else
                {
                    src << methods[i]->getReturntype()->toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::" << methods[i]->getName()->toStdString() << "(" << methods[i]->getArgsAsQString().toStdString() << ")";
                    if (methods[i]->isConst() && !methods[i]->isStatic())
                    {
                        src << " const";
                    }
                    src << endl;
                    src << "{" << endl;
                    src << "\t" << "// method " << methods[i]->getName()->toStdString() << endl;
                    src << "}" << endl << endl;
                }
                header << ";" << endl;
            }
        }
        if (!method_empty) header << endl;
        bool getset_empty(true);
        for (unsigned int i(0);i<members.size();i++)
        {
            if (members[i]->hasGetter())
            {
                QString getname("get");
                QString cpyname(*(members[i]->getName()));
                getname += cpyname.toUpper().at(0);
                getname += cpyname.toLower().remove(0,1);
                header << "\t\t" << members[i]->getType()->toStdString() << " " << getname.toStdString() << "() const {return " << members[i]->getName()->toStdString() << ";}" << endl;
            }
            if (members[i]->hasSetter())
            {
                QString setname("set");
                QString cpyname(*(members[i]->getName()));
                setname += cpyname.toUpper().at(0);
                setname += cpyname.toLower().remove(0,1);
                header << "\t\t" << "void " << setname.toStdString() << "(" << members[i]->getType()->toStdString() << " new_) {" << members[i]->getName()->toStdString() << " = new_;}" << endl;
            }
        }
        if (!getset_empty) header << endl;
        for (unsigned int i(0);i<members.size();i++)
        {
            if (members[i]->getVisibility() == PUBLIC)
            {
                member_empty = false;
                header << "\t\t";
                if (members[i]->isStatic())
                {
                    header << "static ";
                }
                header << members[i]->getType()->toStdString() << " " << members[i]->getName()->toStdString() << ";" << endl;
            }
        }
        if (!member_empty) header << endl;
        header << "\t" << "protected:" << endl;
        method_empty = true;
        member_empty = true;
        for (unsigned int i(0);i<methods.size();i++)
        {
            if (methods[i]->getVisibility() == PROTECTED)
            {
                method_empty = false;
                header << "\t\t";
                if (methods[i]->isStatic())
                {
                    header << "static ";
                }
                else if (methods[i]->isVirtual())
                {
                    header << "virtual ";
                }
                header << methods[i]->getReturntype()->toStdString() << " " << methods[i]->getName()->toStdString() << "(" << methods[i]->getArgsAsQString().toStdString() << ")";
                if (methods[i]->isConst() && !methods[i]->isStatic())
                {
                    header << " const";
                }
                if (methods[i]->isVirtual() && methods[i]->isAbstract() && !methods[i]->isStatic())
                {
                    header << " = 0";
                }
                else
                {
                    src << methods[i]->getReturntype()->toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::" << methods[i]->getName()->toStdString() << "(" << methods[i]->getArgsAsQString().toStdString() << ")";
                    if (methods[i]->isConst() && !methods[i]->isStatic())
                    {
                        src << " const";
                    }
                    src << endl;
                    src << "{" << endl;
                    src << "\t" << "// method " << methods[i]->getName()->toStdString() << endl;
                    src << "}" << endl << endl;
                }
                header << ";" << endl;
            }
        }
        if (!method_empty) header << endl;
        for (unsigned int i(0);i<members.size();i++)
        {
            if (members[i]->getVisibility() == PROTECTED)
            {
                member_empty = false;
                header << "\t\t";
                if (members[i]->isStatic())
                {
                    header << "static ";
                }
                header << members[i]->getType()->toStdString() << " " << members[i]->getName()->toStdString() << ";" << endl;
            }
        }
        if (!member_empty) header << endl;
        header << "\t" << "private:" << endl;
        method_empty = true;
        for (unsigned int i(0);i<methods.size();i++)
        {
            if (methods[i]->getVisibility() == PRIVATE)
            {
                method_empty = false;
                header << "\t\t";
                if (methods[i]->isStatic())
                {
                    header << "static ";
                }
                else if (methods[i]->isVirtual())
                {
                    header << "virtual ";
                }
                header << methods[i]->getReturntype()->toStdString() << " " << methods[i]->getName()->toStdString() << "(" << methods[i]->getArgsAsQString().toStdString() << ")";
                if (methods[i]->isConst() && !methods[i]->isStatic())
                {
                    header << " const";
                }
                if (methods[i]->isVirtual() && methods[i]->isAbstract() && !methods[i]->isStatic())
                {
                    header << " = 0";
                }
                else
                {
                    src << methods[i]->getReturntype()->toStdString() << " " << ((QLineEdit*)widgets["name"])->text().toStdString() << "::" << methods[i]->getName()->toStdString() << "(" << methods[i]->getArgsAsQString().toStdString() << ")";
                    if (methods[i]->isConst() && !methods[i]->isStatic())
                    {
                        src << " const";
                    }
                    src << endl;
                    src << "{" << endl;
                    src << "\t" << "// method " << methods[i]->getName()->toStdString() << endl;
                    src << "}" << endl << endl;
                }
                header << ";" << endl;
            }
        }
        if (!method_empty) header << endl;
        for (unsigned int i(0);i<members.size();i++)
        {
            if (members[i]->getVisibility() == PRIVATE)
            {
                header << "\t\t";
                if (members[i]->isStatic())
                {
                    header << "static ";
                }
                header << members[i]->getType()->toStdString() << " " << members[i]->getName()->toStdString() << ";" << endl;
            }
        }
        header << "};" << endl;
        header << endl;
        header << "#endif // " << ((QLineEdit*)widgets["define"])->text().toStdString() << endl;
        header.close();
        src.close();
        QMessageBox::information(this,"Class generated","Your class has been generated succesful !\nSrc path : "+srcpath+"\nHeader path : "+headerpath);
        qApp->quit();
    }
    else
    {
        QMessageBox::critical(this,"Cannot create files","The files cannot be created, the path seems to be not correct\nSrc path : "+srcpath+"\nHeader path : "+headerpath);
    }
}
