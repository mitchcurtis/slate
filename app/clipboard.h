/*
    Copyright 2016, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FILEVALIDATOR_H
#define FILEVALIDATOR_H

#include <QObject>
#include <QUrl>

class FileValidator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool fileValid READ isFileValid NOTIFY fileValidChanged)
    Q_PROPERTY(QString fileErrorMessage READ fileErrorMessage WRITE setFileErrorMessage NOTIFY fileErrorMessageChanged)
    Q_PROPERTY(bool treatAsImage READ treatAsImage WRITE setTreatAsImage NOTIFY treatAsImageChanged)

public:
    explicit FileValidator(QObject *parent = 0);

    QUrl url() const;
    void setUrl(const QUrl &url);

    bool isFileValid() const;

    QString fileErrorMessage() const;
    void setFileErrorMessage(const QString &fileErrorMessage);

    bool treatAsImage() const;
    void setTreatAsImage(bool treatAsImage);

signals:
    void urlChanged();
    void fileValidChanged();
    void fileErrorMessageChanged();
    void treatAsImageChanged();

protected:
    virtual void validate();

    QUrl mUrl;
    QString mFileErrorMessage;
    bool mTreatAsImage;
};

#endif // FILEVALIDATOR_H
