#ifndef FILESBOX_H
#define FILESBOX_H


#include <QListWidget>
#include <QListView>
#include <QFileSystemModel>

#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSpinBox>


enum mediaType{
    video,
    audio,
    subs
};

class FilesBox : public QWidget
{
    Q_OBJECT

public:
    explicit FilesBox(mediaType type, QWidget *parent = 0);
    ~FilesBox();

    QWidget *getUi();
    QListView *getView();

    void setNameBox(QString nameBox);
    void setRootPath(QString path);



    QString getLang();
    QString getCharset();
    QString getName();
    QString getSync();

    QString getPathToFolder();
    QString getPathToFile(int row);
    QString getFileName(int row);
    mediaType getMediaType();



    int getFilesCount ();

public slots:

    void changeFilter(QString filter);
    void openFolder();
    void closeButtonClicked();

    void setItemSelected(int item);
    void indexChanged(QModelIndexList);
    void useVideoFolderSignal();

signals:

    void selectItem(int);
    void useVideoFolder(FilesBox*);
    void removeMe(FilesBox*);

private slots:


private:

    QFileSystemModel *fsmodel;
    QListView *listView;

    QStringList pathsFiles;
    QString sDir;
    mediaType boxType;

    QWidget *box;
    QDir dirFiles;


    void createVideoBox();
    void createAudioBox();
    void createSubsBox();

    void addButtons();
    void addCharset();
    void addNameAndLang();    
    void addFilter();
    void addSync();

    void addVideoAudioSpacer();

    void setSelected (int n);

    QVBoxLayout *boxLayout, *vert2, *vert3;

    QHBoxLayout *codepageLayout, *buttonLayout, *checkBoxLayout,
    *filterLayout, *nameLayout, *langLayout, *syncLayout;

    QPushButton *folderButton, *closeButton;

    QLineEdit *filterEdit, *nameEdit;

    QComboBox *charsetBox, *langBox;

    QLabel *nameLabel, *filterLabel, *codepageLabel, *langLabel,
        *syncLabel, *selectedLabel;

    QCheckBox *useVideoFolderCheckBox;

    QSpacerItem *filterH, *codePageH, *nameHS, *langHS,
        *buttonHS1, *buttonHS2, *buttonHS3, *videoAudioV, *syncH, *selectH;

    QSpinBox *syncBox;

};

#endif // FILESBOX_H
