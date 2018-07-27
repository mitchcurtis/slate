#ifndef GUIDEMODEL_H
#define GUIDEMODEL_H

#include <QAbstractListModel>

class Project;

class GuideModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)

public:
    explicit GuideModel(QObject *parent = nullptr);

    enum {
        XPositionRole,
        YPositionRole,
        OrientationRole
    };

    Project *project() const;
    void setProject(Project *project);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

signals:
    void projectChanged();

private slots:
    void onPreGuideAppended();
    void onPostGuideAppended();
    void onGuidePositionChanged(int index);
    void onPreGuideRemoved(int index);
    void onPostGuideRemoved();

private:
    Project *mProject = nullptr;
};

#endif // GUIDEMODEL_H
