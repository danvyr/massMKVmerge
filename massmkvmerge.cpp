#include "massmkvmerge.h"
#include "ui_massmkvmerge.h"

#include <QFileDialog>

#ifdef QT_DEBUG

#include <QDebug>

#endif


MassMKVmerge::MassMKVmerge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MassMKVmerge)
{
    ui->setupUi(this);

    videoBox = new FilesBox(video);
    ui->horizontalLayout->addWidget(videoBox->getUi());

    ui->saveToResult->setText(QDir::currentPath());

    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startMerge()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopMerge()));
    connect(ui->addSubs, SIGNAL(clicked()), this, SLOT(addSubs()));
    connect(ui->addAudio, SIGNAL(clicked()), this, SLOT(addAudio()));

    connect(ui->editCmdButton, SIGNAL(clicked()), this, SLOT(editCMD()));
    connect(ui->saveCmd, SIGNAL(clicked()), this, SLOT(saveCmdToFile()));

    connect(ui->saveToButton, SIGNAL(clicked()), this, SLOT(saveTo()));

    connect(&mergeThread, SIGNAL(mergeProgress(int)), this, SLOT(setProgressBar(int)));
    connect(this, SIGNAL(mergeStop()), &mergeThread, SLOT(stopMerge()));

    subPath ="SUB"; audioPath = "AUDIO";
    outPath="OUT_DIR"; videoPath = "VIDEO"; dirSuffix="_DIR";
}

void MassMKVmerge::addSubs()
{
    FilesBox *temp = new FilesBox(subs);
    subsList.append(temp);
    ui->horizontalLayout->addWidget(subsList.last()->getUi());   
    connect(subsList.last(), SIGNAL(useVideoFolder(FilesBox*)), this, SLOT(setVideRootPath(FilesBox*)));
    connect(subsList.last(), SIGNAL(removeMe(FilesBox*)), this, SLOT(deleteMediaBox(FilesBox*)));
}

void MassMKVmerge::addAudio()
{
    FilesBox *temp = new FilesBox(audio);
    audioList.append(temp);
    ui->horizontalLayout->addWidget(temp->getUi());
    connect(audioList.last(), SIGNAL(useVideoFolder(FilesBox*)), this, SLOT(setVideRootPath(FilesBox*)));
    connect(audioList.last(), SIGNAL(removeMe(FilesBox*)), this, SLOT(deleteMediaBox(FilesBox*)));
}

void MassMKVmerge::deleteMediaBox(FilesBox* tempBox)
{
    if(tempBox->getMediaType() ==audio)
    {
        audioList.removeOne(tempBox);
    }
    else if (tempBox->getMediaType() == subs)
    {
        subsList.removeOne(tempBox);
    }
}


void MassMKVmerge::saveTo()
{
    ui->saveToResult->setText(QFileDialog::getExistingDirectory(this, tr("Save result to"),
                                                                      "~/",
                                                                      QFileDialog::ShowDirsOnly |
                                                                      QFileDialog::DontResolveSymlinks));
}

QString MassMKVmerge::generateCMD(int row, bool pathWithFolders = true)
{

    QString commandLine="mkvmerge ";

    // output
    if(pathWithFolders)
    {
        commandLine+=" -o \"" + ui->saveToResult->text().toLocal8Bit() + "/"
                +videoBox->getFileName(row).remove(".avi")+".mkv\" ";

        commandLine+='\"'+videoBox->getPathToFile(row)+'\"';
    }
    else
    {
        commandLine+=" -o \"$" + outPath + QDir::separator()
                +videoBox->getFileName(row).remove(".avi")+".mkv\" ";

        commandLine+="\"$"+ videoPath+'0'+dirSuffix + QDir::separator()
                +videoBox->getFileName(row)+"\" ";
    }


    //add audio
    if(audioList.isEmpty() == false )
    {
        for(int i = 0; i < audioList.count(); i++)
        {
            commandLine+=generateMediaOptions(audioList.at(i), i , row, pathWithFolders);
        }
    }

    //add subs
    if(subsList.isEmpty() == false )
    {
        for(int i = 0; i < subsList.count(); i++)
        {
            commandLine+=generateMediaOptions(subsList.at(i), i ,row, pathWithFolders);
        }
    }

#ifdef QT_DEBUG
        qDebug()<< commandLine;
#endif

    mergeCmd.append(commandLine);
    return commandLine;
}

QString MassMKVmerge::generateMediaOptions(FilesBox *tempBox,
                                           int num,
                                           int row,
                                           bool pathWithFolders)
{
    QString str;

    str= writeLang(num, tempBox);

    if (tempBox->getName() != "")
        str+= writeName(num, tempBox);

    if (tempBox->getSync() != "0")
        str+= writeTiming(num, tempBox);

    if(tempBox->getMediaType() == subs && !pathWithFolders)
    {
        str+= writeCharset(num, tempBox);
    }

    if (pathWithFolders)
        str+='\"'+tempBox->getPathToFile(row)+'\"';
    else
    {

        QString mStr;
        if (tempBox->getMediaType() == subs)
        {
            str+="--sub-charset "+QString::number(num)+':' + "UTF-8  ";
            mStr=subPath;
            QString oldFileName="\"$" + mStr+QString::number(num)+dirSuffix+ QDir::separator()
                    + tempBox->getFileName(row)+"\" ";
            QString newFileName= "\"$" + mStr+QString::number(num)+dirSuffix+ QDir::separator()
                    + tempBox->getFileName(row).insert( tempBox->getFileName(row).count() - 3 ,"UTF8")+"\" ";

            if(tempBox->getCharset() != "UTF-8")
            {
                iconvCmd.append("iconv -f "+ tempBox->getCharset() + " -t utf8 " + oldFileName + " -o " + newFileName);

                str+=newFileName;
            }
            else
            {
                str+=oldFileName;
            }
        }
        else if (tempBox->getMediaType() == audio)
        {
            mStr=audioPath;


        str+="\"$" + mStr+QString::number(num)+dirSuffix+ QDir::separator()
                + tempBox->getFileName(row)+'\"';
        }
    }

    return str;
    /*


    "mkvmerge" -o "/media/server/serials/Friends/Friends Bonus/Friends - All The Gag Reels.mkv"

    "--forced-track" "0:no" "--language" "1:eng" "--forced-track" "1:no" "-a" "1" "-d" "0" "-S" "-T"
    "--no-global-tags" "--no-chapters"
    "(" "/media/server/serials/Friends/Friends Bonus/Friends - All The Gag Reels.avi" ")"


    "--language" "0:eng" "--track-name" "0:English" "--forced-track" "0:no" "-s" "0" "-D" "-A" "-T" "--no-global-tags"
    "--no-chapters"
    "(" "/media/server/serials/Friends/Friends Bonus/Friends - All The Gag Reels (Eng).srt" ")"


    "--sub-charset" "0:UTF-8" "--language" "0:rus" "--sync" "0:0" "--track-name" "0:russian" "--forced-track"
    "0:no" "-s" "0" "-D" "-A" "-T" "--no-global-tags" "--no-chapters"
    "(" "/media/server/serials/Friends/Friends Bonus/Friends - All The Gag Reels (Rus).srt" ")"


    "--track-order" "0:0,0:1,1:0,2:0"

    file:subfile
    */
}



QString MassMKVmerge::writeLang(int n, FilesBox *tempBox)
{
    return  "--language "+QString::number(n)+':' + tempBox->getLang()+" ";
}

QString MassMKVmerge::writeCharset(int n, FilesBox *tempBox)
{
    return  "--sub-charset "+QString::number(n)+':' + tempBox->getCharset()+" ";
}

QString MassMKVmerge::writeName(int n,  FilesBox *tempBox)
{
    return  "--track-name "+QString::number(n)+':' + tempBox->getName()+" ";
}

QString MassMKVmerge::writeTiming(int n,  FilesBox *tempBox)
{
    return  "--sync "+QString::number(n)+':' + tempBox->getSync()+" ";
}



void MassMKVmerge::startMerge()
{
    iconvCmd.clear();
    mergeCmd.clear();
    int  rows = videoBox->getFilesCount();

#ifdef QT_DEBUG
        qDebug()<<"rows " << rows;
#endif

        QStringList temp;
        for (int row = 0; row < rows; row++)
        {
             temp.append(generateCMD(row));
        }
//        mergeThread = new MergeFiles();
        mergeThread.setCommandLines(temp);


        mergeThread.run();
    //}
}

void MassMKVmerge::setProgressBar(int i)
{
    ui->progressBar->setValue(i);
}


void MassMKVmerge::editCMD()
{
    editCmdDialog = new QDialog();

    editCmdBoxLayout = new QVBoxLayout;
    editCmdDialog->setLayout(editCmdBoxLayout);

    currentCmd = new QLabel("Current cmd for 1st file:",editCmdDialog);
    additionalCmd = new QLabel("Add cmd:", editCmdDialog);
    currentCmdEdit = new QPlainTextEdit(editCmdDialog);
    addCmdForVideoEdit = new QPlainTextEdit(editCmdDialog);
    addCmdForFileEdit = new QPlainTextEdit(editCmdDialog);

    dialogButton = new QDialogButtonBox(editCmdDialog);


    currentCmdEdit->setReadOnly(true);
    currentCmdEdit->setPlainText(generateCMD(0));

    dialogButton->setStandardButtons(QDialogButtonBox::Ok|
                                     QDialogButtonBox::Cancel);


    editCmdBoxLayout->addWidget(currentCmd);
    editCmdBoxLayout->addWidget(currentCmdEdit);
    editCmdBoxLayout->addWidget(additionalCmd);
    editCmdBoxLayout->addWidget(addCmdForVideoEdit);
    editCmdBoxLayout->addWidget(addCmdForFileEdit);
    editCmdBoxLayout->addWidget(dialogButton);

    editCmdDialog->show();

    connect(dialogButton, SIGNAL(clicked(QAbstractButton*)),
                                 this, SLOT(editCmdClicked(QAbstractButton*)));

}
void MassMKVmerge::editCmdClicked(QAbstractButton* button)
{
    if(button == dialogButton->button(QDialogButtonBox::Ok))
    {
        editCmdDialog->~QDialog();

    }
    else if (button ==  dialogButton->button(QDialogButtonBox::Cancel) )
    {
        editCmdDialog->~QDialog();
    }
}

void MassMKVmerge::saveCmdToFile()
{
    iconvCmd.clear();
    mergeCmd.clear();
    QString pathToFile = QFileDialog::getSaveFileName(this, tr("Save result to"),
                                                    #ifdef Q_OS_LINUX
                                                      "merge.sh",
                                                      tr("*.sh"));
                                                    #endif
                                                    #ifdef Q_OS_WIN
                                                      "merge.sh",
                                                      tr("*.cmd"));
                                                    #endif


    if( ! pathToFile.isEmpty() )
    {
        int  rows = videoBox->getFilesCount();
        QFile sFile(pathToFile);

        if( sFile.open(QIODevice::WriteOnly) )
        {
//            QByteArray array;
//            array.


            QTextStream outstream(&sFile);
//            outstream << "\#!/bin/bash"+'\n'+'\n';

            outstream << outPath + "="  + '\"' + ui->saveToResult->text() +'\"' +'\n';
            outstream << videoPath + QString::number(0)+dirSuffix + "="  + '\"' + videoBox->getPathToFolder() +'\"' +'\n';


            //ADD FOLDERS
            if(audioList.isEmpty() == false )
            {
                for(int i = 0; i < audioList.count(); i++)
                {
                    outstream << audioPath + QString::number(i)+dirSuffix+"=" + '\"' + audioList.at(i)->getPathToFolder() +'\"' +'\n';
                }
            }
            if(subsList.isEmpty() == false )
            {
                for(int i = 0; i < subsList.count(); i++)
                {
                    outstream << subPath + QString::number(i)+dirSuffix+"=" + '\"' + subsList.at(i)->getPathToFolder() +'\"' +'\n';
                }
            }


            rows = videoBox->getFilesCount();

            outstream << "\n\n";

            for (int row = 0; row < rows; row++)
            {
                generateCMD(row, false);
            }

            QString ts;
            for (int i = 0; i < iconvCmd.count(); i++)
                outstream << iconvCmd.at(i)+'\n';

            outstream << "\n\n";

            for (int i = 0; i < mergeCmd.count(); i++)
                outstream << mergeCmd.at(i)+'\n';

            sFile.close();
        }
    }
/*
 *
VIDEO1_DIR="/media/server/serials/not_sort/Cheers/Season 7";
SUB1_DIR="/media/server/serials/not_sort/Cheers/Season 7";
OUT_DIR="/home/common/testmkv";

mkvmerge -o "$OUT_DIR/01. How To Recede In Business.mkv"  "--forced-track" "0:no"
"--language" "1:eng" "--forced-track" "1:no" "-a" "1" "-d" "0" "-S" "-T"
"--no-global-tags" "--no-chapters"

"(" "$VIDEO1_DIR/01. How To Recede In Business.avi"
")" "--language" "0:eng" "--forced-track" "0:no" "-s" "0" "-D" "-A" "-T"
"--no-global-tags" "--no-chapters"

"(" "$SUB1_DIR/Cheers 1988 s07 (01).srt" ")"
"--track-order" "0:0,0:1,1:0" ;


*/
}


void MassMKVmerge::stopMerge()
{
    emit mergeStop();
}

void MassMKVmerge::setVideRootPath(FilesBox *tempBox)
{
    tempBox->setRootPath(videoBox->getPathToFolder());

}


MassMKVmerge::~MassMKVmerge()
{
    delete videoBox;
    delete ui;
}
