#include "mergefiles.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


MergeFiles::MergeFiles(QThread *parent) :
    QThread(parent)
{


}


void MergeFiles::run()
{
    QString cmd;
    for(int i =0; i < cmdList->count(); i++ )
    {
        if(mergeContinue)
        {

#ifdef QT_DEBUG
            qDebug()<<cmdList->at(i);
#endif

#ifdef Q_OS_LINUX
            system(cmdList->at(i).toLocal8Bit());
#endif

#ifdef Q_OS_WIN
            QProcess merge= new QProcess;
            merge->start("explorer " + commandLine.toLocal8Bit());
#endif
            emit mergeProgress( (int) 100*(i+1)/cmdList->count() );

        }

    }
}


void MergeFiles::stopMerge()
{
    mergeContinue = false;
}

void MergeFiles::setCommandLines(QStringList cList)
{
    cmdList = &cList;
}

MergeFiles::~MergeFiles()
{
}
