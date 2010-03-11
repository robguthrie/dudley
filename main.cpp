#include <QtCore/QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include "dudleylog.h"
/* first up. recurse through directorys from current and list all the files and their mtimes */

/* ok so want direcdtory walker function.. pass in a QFileInfoList and it will 
  populate it with all the files in the directory recursively */

void scanDirectory(QList<QFileInfo> *fileInfoList, QString path)
{
  QDir dir(path);
  dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
  dir.setSorting(QDir::Size | QDir::Reversed);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);
    /* if fileInfo refers to a directory, need to recurse into the directory, 
       passing the address of the QFileInfoList to the recurring fuction */
    if (!fileInfo.isDir()) {
      fileInfoList->append(fileInfo);
    }else{
      //std::cout << qPrintable(fileInfo.filePath()) << std::endl;
      scanDirectory(fileInfoList, fileInfo.filePath());
    }
  }
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString logFile(argv[1]);
    logFile += "/dudley.log";
    DudleyLog dlog(logFile);

//  std::cout << "Scanning directory: " << argv[1] << "\n";
//  QList<QFileInfo> fileInfoList;
//  scanDirectory(&fileInfoList, argv[1]);
//  for(int i = 0; i < fileInfoList.size(); i++){
//      std::cout << qPrintable(fileInfoList.at(i).absoluteFilePath()) << std::endl;
//  }
  return 0;
}

