#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QtQml>
#include <QIcon>
#include <QUrl>

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        DisplayRole = Qt::DisplayRole,
        EditRole = Qt::EditRole,
        DecorationRole = Qt::DecorationRole,
        ChildCountRole = Qt::UserRole + 1
    };

    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel();
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    
    // 添加这些方法用于QML调用
    Q_INVOKABLE bool addItem(const QModelIndex &parentIndex, const QString &data);
    Q_INVOKABLE bool removeItem(const QModelIndex &index);
    Q_INVOKABLE QModelIndex getIndex(int row, int column, const QModelIndex &parent = QModelIndex()) const {
        return index(row, column, parent);
    }
    
    // 获取节点的完整路径（从根节点到当前节点）
    Q_INVOKABLE QStringList getNodePath(const QModelIndex &index) const;

    QModelIndex parent(const QModelIndex &index) const override;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    void createModelData(TreeItem *parent);  
    // 递归计算子节点总数（包括嵌套子节点）
    int calculateTotalChildCount(TreeItem *item) const;
    // 获取节点的深度（层级）
    int getNodeDepth(const QModelIndex &index) const;
    // 根据节点类型获取适当的图标URL
    QUrl getIconForNode(const QModelIndex &index) const;
    
    TreeItem *rootItem;
};

#endif // TREEMODEL_H
