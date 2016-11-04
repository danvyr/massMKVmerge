#ifndef MERGEFILES_H
#define MERGEFILES_H

#include <QThread>
#include <QProcess>


class MergeFiles : public QThread
{
    Q_OBJECT

public:
    explicit MergeFiles(QThread *parent = 0);
    ~MergeFiles();

    void run();

    void setCommandLines(QStringList cList);



public slots:
    void stopMerge();



signals:

    void mergeEnd();
    void mergeProgress(int);

private:

    bool mergeContinue;

    QProcess *merge;
    QStringList *cmdList;

};

#endif // MERGEFILES_H
