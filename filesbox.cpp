#include "filesbox.h"

#include <QFileDialog>

#ifdef QT_DEBUG

#include <QDebug>

#endif

FilesBox::FilesBox(mediaType type, QWidget *parent) :
    QWidget(parent)
{

    boxType = type;

    box = new QWidget;
    box->setMinimumSize(250,320);

    boxLayout = new QVBoxLayout(box);

    switch (type) {
    case audio:
        createAudioBox();
        break;
    case subs:
        createSubsBox();
#ifdef QT_DEBUG
        qDebug()<< nameEdit->size().height();
        qDebug()<< langBox->size().height();
        qDebug()<< charsetBox->size().height();
        qDebug()<< filterEdit->size().height();
#endif
        break;
    case video:
        createVideoBox();
        break;
    }

    sDir = "/media/data/Season 8";//QDir::currentPath();
    fsmodel = new QFileSystemModel(box);
    fsmodel->setFilter( QDir::NoDotAndDotDot | QDir::Files);
    fsmodel->setRootPath(sDir);


    listView = new QListView(box);
    listView->setModel(fsmodel);
    listView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);

    listView->setRootIndex(fsmodel->setRootPath(sDir));

    boxLayout->addWidget(listView);

    setSelected(getFilesCount());


    connect(filterEdit, SIGNAL(textChanged(QString)),
            this, SLOT(changeFilter(QString)));

    connect(folderButton, SIGNAL(clicked()), this, SLOT(openFolder()));

    connect(listView, SIGNAL(indexesMoved(QModelIndexList)),
            this,SLOT(indexChanged(QModelIndexList)));


    if (boxType != video)
    {
        connect(closeButton, SIGNAL(clicked()),
                this, SLOT(closeButtonClicked()));
    }
}

void FilesBox::setRootPath(QString path)
{
    sDir=path;
    listView->setRootIndex(fsmodel->setRootPath(sDir));
    setSelected(getFilesCount());
}

void FilesBox::openFolder()
{

    setRootPath(QFileDialog::getExistingDirectory(this,
                                                  tr("Open Directory"),
                                                    "~/",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks));
}




void FilesBox::closeButtonClicked()
{
    emit removeMe(this);
    box->~QWidget();
}

QString FilesBox::getPathToFile(int fileNumber)
{
    return dirFiles.path()+ QDir::separator()+ pathsFiles.at(fileNumber);
}

QString FilesBox::getFileName(int row)
{
    return pathsFiles.at(row);
}

int FilesBox::getFilesCount ()
{
    QStringList filtersList;
    filtersList<< filterEdit->text();

    dirFiles.setPath(sDir);
    pathsFiles = dirFiles.entryList(filtersList,
                       QDir::Files | QDir::NoDotAndDotDot);
//                       QDir::Name);
    return pathsFiles.count();
}

void FilesBox::setSelected (int n)
{
    selectedLabel->setText("Selected: " + QString::number(n) + " files");
}

void FilesBox::changeFilter(QString filter)
{
    QStringList tempStrList;
    tempStrList.append(filter);
    fsmodel->setNameFilters(tempStrList);
    fsmodel->setNameFilterDisables(false);
    setSelected(getFilesCount());
}

void FilesBox::setItemSelected(int item)
{

}

void FilesBox::indexChanged(QModelIndexList)
{

}

QString FilesBox::getLang()
{
    return langBox->currentText();
}

QString FilesBox::getCharset()
{
    return charsetBox->currentText();
}

QString FilesBox::getName()
{
    return nameEdit->text();
}

QString FilesBox::getSync()
{
    return QString::number(syncBox->value());
}


void FilesBox::createVideoBox()
{
    addButtons();
    addFilter();
    addVideoAudioSpacer();
}

void FilesBox::createAudioBox()
{
    addButtons();
    addSync();
    addNameAndLang();
    addFilter();
    addVideoAudioSpacer();
}

void FilesBox::createSubsBox()
{
    addButtons();
    addSync();
    addNameAndLang();
    addCharset();
    addFilter();
}

void FilesBox::addCharset()
{
    codepageLayout = new QHBoxLayout();

    codePageH = new QSpacerItem(40, 10, QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    charsetBox = new QComboBox(box);
    charsetBox->addItem("UTF-8");
    charsetBox->addItem("CP-1251");

    charsetBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    codepageLabel = new QLabel(box);
    codepageLabel->setText(QStringLiteral("Charset: "));

    codepageLayout->addWidget(codepageLabel);
    codepageLayout->addWidget(charsetBox);
    codepageLayout->addSpacerItem(codePageH);

    boxLayout->addLayout(codepageLayout);



}

void FilesBox::addNameAndLang()
{
    nameLayout = new QHBoxLayout();

    nameHS = new QSpacerItem(40, 10, QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    nameEdit = new QLineEdit(box);
    nameLabel = new QLabel(box);
    nameLabel->setText(QStringLiteral("Name: "));

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameLayout->addSpacerItem(nameHS);


    langLayout = new QHBoxLayout();

    langHS = new QSpacerItem(40, 10, QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    langBox = new QComboBox(box);
    langBox->addItem("eng");
    langBox->addItem("rus");


    langLabel = new QLabel(box);
    langLabel->setText(QStringLiteral("Language: "));

    langLayout->addWidget(langLabel);
    langLayout->addWidget(langBox);
    langLayout->addSpacerItem(langHS);

    boxLayout->addLayout(nameLayout);
    boxLayout->addLayout(langLayout);
}


void FilesBox::addSync()
{
    syncLayout = new QHBoxLayout();

    syncBox = new QSpinBox(box);
    syncBox->setFixedWidth(100);
    syncBox->setSuffix(" msec");
    syncBox->setMinimum(-10000);
    syncBox->setMaximum(10000);
    syncBox->setSingleStep(250);

    syncH = new QSpacerItem(40, 10, QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    syncLabel = new QLabel(box);
    syncLabel->setText("Sync: ");

    syncLayout->addWidget(syncLabel);
    syncLayout->addWidget(syncBox);
    syncLayout->addSpacerItem(syncH);

    boxLayout->addLayout(syncLayout);
}

void FilesBox::addFilter()
{
    filterLayout = new QHBoxLayout();

    filterH = new QSpacerItem(40, 10, QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    selectH = new QSpacerItem(20, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
    filterEdit = new QLineEdit(box);
    filterEdit->setText("*.*");
    filterLabel = new QLabel(box);
    filterLabel->setText(QStringLiteral("Filter: "));

    selectedLabel = new QLabel(box);

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterEdit);
    filterLayout->addSpacerItem(selectH);
    filterLayout->addWidget(selectedLabel);
    filterLayout->addSpacerItem(filterH);

    boxLayout->addLayout(filterLayout);
}


void FilesBox::addButtons()
{
    buttonLayout = new QHBoxLayout();


    folderButton = new QPushButton(box);
    folderButton->setText("Folder");

    buttonLayout->addWidget(folderButton);

    if (boxType != video)
    {
        buttonHS1 = new QSpacerItem(10, 10, QSizePolicy::Fixed,
                                         QSizePolicy::Fixed);
        buttonLayout->addSpacerItem(buttonHS1);

        useVideoFolderCheckBox = new QCheckBox(box);
        useVideoFolderCheckBox->setText("Use film folder");

        closeButton = new QPushButton(box);
        closeButton->setText("X");
        closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        closeButton->resize(15,15);

        buttonHS2 = new QSpacerItem(10, 10, QSizePolicy::Fixed,
                                  QSizePolicy::Fixed);
        buttonHS3 = new QSpacerItem(40, 10, QSizePolicy::Expanding,
                                  QSizePolicy::Fixed);

        buttonLayout->addWidget(useVideoFolderCheckBox);
        buttonLayout->addSpacerItem(buttonHS2);

        buttonLayout->addWidget(closeButton);
        buttonLayout->addSpacerItem(buttonHS3);

        connect(useVideoFolderCheckBox, SIGNAL(clicked()),
                this, SLOT(useVideoFolderSignal()));
    }
    else
    {
        buttonHS1 = new QSpacerItem(10, 10, QSizePolicy::Expanding,
                                         QSizePolicy::Fixed);
        buttonLayout->addSpacerItem(buttonHS1);
    }

    boxLayout->addLayout(buttonLayout);
}

void FilesBox::addVideoAudioSpacer()
{
    if (boxType == video)
    {
        videoAudioV = new QSpacerItem(10, 108, QSizePolicy::Fixed,
                                      QSizePolicy::Fixed);

    }
    else if (boxType == audio)
    {
        videoAudioV = new QSpacerItem(10, 27, QSizePolicy::Fixed,
                                      QSizePolicy::Minimum);

    }
    boxLayout->addSpacerItem(videoAudioV);
}

mediaType FilesBox::getMediaType()
{
    return boxType;
}

void FilesBox::useVideoFolderSignal()
{
    if(useVideoFolderCheckBox->isChecked())
    {
        emit useVideoFolder(this);
        folderButton->setDisabled(true);
        setSelected(getFilesCount());
    }
    else
    {
        folderButton->setDisabled(false);
    }
}


void FilesBox::setNameBox(QString nameBox)
{
    box->setObjectName(nameBox);
}

QString FilesBox::getPathToFolder()
{
    return sDir;
}

QWidget *FilesBox::getUi()
{
    return box;
}

QListView *FilesBox::getView()
{
    return listView;
}


FilesBox::~FilesBox()
{
    delete box;
}
