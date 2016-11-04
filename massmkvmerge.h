#ifndef MASSMKVMERGE_H
#define MASSMKVMERGE_H

#include <QMainWindow>
#include <QList>
#include <QPlainTextEdit>
#include <QDialogButtonBox>

#include "filesbox.h"
#include "mergefiles.h"


namespace Ui {
class MassMKVmerge;
}

class MassMKVmerge : public QMainWindow
{
    Q_OBJECT

public:
    explicit MassMKVmerge(QWidget *parent = 0);
    ~MassMKVmerge();

signals:
    void mergeStop();


public slots:
    void addSubs();
    void addAudio();
    void startMerge();
    void stopMerge();

    void setVideRootPath(FilesBox*);
    void deleteMediaBox(FilesBox*tempBox);

    void editCMD();

    void saveTo();

    void setProgressBar(int i);

    void editCmdClicked(QAbstractButton*button);
    void saveCmdToFile();

private:

    MergeFiles mergeThread;

    QDialog *editCmdDialog;
    QVBoxLayout *editCmdBoxLayout;
    QLabel *currentCmd, *additionalCmd;
    QPlainTextEdit *currentCmdEdit, *addCmdForVideoEdit, *addCmdForFileEdit ;
    QDialogButtonBox *dialogButton;


    QString generateCMD(int row, bool pathWithFolders);
    QString generateMediaOptions(FilesBox *tempBox,
                                 int num,
                                 int row,
                                 bool pathWithFolders);

    QString writeLang(      int n, FilesBox *tempBox);
    QString writeCharset(   int n, FilesBox *tempBox);
    QString writeName(      int n, FilesBox *tempBox);
    QString writeTiming(    int n, FilesBox *tempBox);


    QString subPath, audioPath, outPath, videoPath, dirSuffix;
    QStringList iconvCmd, mergeCmd;


    Ui::MassMKVmerge *ui;
    FilesBox *videoBox;

    int currentPos;

//    QSpacerItem *space;

    QList <FilesBox*> subsList, audioList;

};

#endif // MASSMKVMERGE_H
