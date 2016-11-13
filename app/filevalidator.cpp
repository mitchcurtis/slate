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

#include "filevalidator.h"

#include <QFile>
#include <QImage>

FileValidator::FileValidator(QObject *parent) :
    QObject(parent),
    mTreatAsImage(false)
{
    setFileErrorMessage("Must specify a file");
}

QUrl FileValidator::url() const
{
    return mUrl;
}

void FileValidator::setUrl(const QUrl &url)
{
    if (url == mUrl)
        return;

    mUrl = url;

    if (mUrl.isEmpty()) {
        setFileErrorMessage(tr("Must specify a file"));
    } else if (!QFile::exists(mUrl.toLocalFile())) {
        setFileErrorMessage(tr("File doesn't exist"));
    } else {
        if (mTreatAsImage) {
            QImage image(mUrl.toLocalFile());
            if (image.isNull()) {
                setFileErrorMessage(tr("Image can not be opened"));
            } else {
                // The image was loaded successfully, so we can clear
                // whatever was here before.
                setFileErrorMessage(QString());
            }
        } else {
            // The file was loaded successfully.
            setFileErrorMessage(QString());
        }
    }

    if (mFileErrorMessage.isEmpty()) {
        // Let derived classes check for problems.
        validate();
    }
    emit urlChanged();
}

bool FileValidator::isFileValid() const
{
    return mFileErrorMessage.isEmpty();
}

QString FileValidator::fileErrorMessage() const
{
    return mFileErrorMessage;
}

void FileValidator::setFileErrorMessage(const QString &fileErrorMessage)
{
    if (fileErrorMessage == mFileErrorMessage)
        return;

    bool wasValid = isFileValid();

    mFileErrorMessage = fileErrorMessage;
    if (isFileValid() != wasValid) {
        emit fileValidChanged();
    }

    emit fileErrorMessageChanged();
}

bool FileValidator::treatAsImage() const
{
    return mTreatAsImage;
}

void FileValidator::setTreatAsImage(bool treatAsImage)
{
    if (treatAsImage == mTreatAsImage)
        return;

    mTreatAsImage = treatAsImage;
    emit treatAsImageChanged();
}

void FileValidator::validate()
{
}
