#include "treemodel.h"
#include "treeitem.h"

TreeModel::TreeModel(QObject* parent) : QAbstractItemModel(parent) {
    rootItem = new TreeItem({tr("Simple Tree Model")});
    createModelData(rootItem);
}

TreeModel::~TreeModel() {
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QHash<int, QByteArray> TreeModel::roleNames() const {
    QHash<int, QByteArray> mapping{
        {Qt::DisplayRole, "displaying"}, 
        {Qt::EditRole, "edition"},
        {Qt::DecorationRole, "decoration"},
        {ChildCountRole, "childCount"}
    };
    return mapping;
}

QVariant TreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return item->data(index.column());
    
    if (role == ChildCountRole) {
        return calculateTotalChildCount(item);
    }
    
    if (role == Qt::DecorationRole) {
        return getIconForNode(index);
    }

    return QVariant();
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem* parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex& parent) const {
    TreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::createModelData(TreeItem* parent) {
    QList<TreeItem*> parents;
    parents << parent;
    QList<QVariant> columnData;

    // 创建一个可见的根节点
    columnData.clear();
    columnData << "病害标注系统";
    TreeItem* visibleRootItem = new TreeItem(columnData, parent);
    parent->appendChild(visibleRootItem);
    
    // 创建主要类别
    QStringList categories = {"泛型", "水体", "电讯", "管线", "气体"};
    
    // 特征绘制类型：Pipe 管道，Point 点，Area 面
    QStringList featureTypes = {"Pipe", "Point", "Area"};

    // 为每个类别创建节点并添加特征绘制类型子节点
    for (const QString& category : categories) {
        columnData.clear();
        columnData << category;
        TreeItem* categoryItem = new TreeItem(columnData, visibleRootItem);
        visibleRootItem->appendChild(categoryItem);

        // 为每个类别添加特征绘制类型子节点
        for (const QString& featureType : featureTypes) {
            columnData.clear();
            columnData << featureType;
            TreeItem* featureItem = new TreeItem(columnData, categoryItem);
            categoryItem->appendChild(featureItem);
        }
    }
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid())
        return false;

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return false;

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    item->setData(index.column(), value);
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    else
        return Qt::ItemIsEditable;
}

bool TreeModel::addItem(const QModelIndex& parentIndex, const QString& data) {
    // 如果没有指定父项，或者父项无效，返回false
    if (!parentIndex.isValid())
        return false;

    TreeItem* parentItem = static_cast<TreeItem*>(parentIndex.internalPointer());
    
    // 获取父节点的层级
    // 现在树的结构是: 
    // rootItem (不可见)
    //   -> 病害标注系统 (可见根节点)
    //       -> 类别 (泛型, 水体等)
    //           -> 特征绘制类型 (Pipe, Point, Area)
    
    // 检查当前节点是否是类别节点（它的父节点应该是可见根节点，其父节点是真正的rootItem）
    TreeItem* grandParentItem = parentItem->parentItem(); // 父节点的父节点
    
    if (grandParentItem && grandParentItem->parentItem() == rootItem) {
        QList<QVariant> itemData;
        itemData << data;
        beginInsertRows(parentIndex, parentItem->childCount(), parentItem->childCount());
        parentItem->appendChild(new TreeItem(itemData, parentItem));
        endInsertRows();

        // 更新节点和所有父节点的计数显示
        QModelIndex currentIndex = parentIndex;
        while (currentIndex.isValid()) {
            emit dataChanged(currentIndex, currentIndex, {ChildCountRole});
            currentIndex = parent(currentIndex);
        }
        
        return true;
    }

    return false;
}

bool TreeModel::removeItem(const QModelIndex& index) {
    if (!index.isValid())
        return false;

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = item->parentItem();

    if (!parentItem)
        return false;

    // 获取父索引用于稍后更新
    QModelIndex parentIndex = parent(index);
    
    beginRemoveRows(parentIndex, index.row(), index.row());
    parentItem->removeChild(index.row());
    endRemoveRows();

    // 更新节点和所有父节点的计数显示
    QModelIndex currentIndex = parentIndex;
    while (currentIndex.isValid()) {
        emit dataChanged(currentIndex, currentIndex, {ChildCountRole});
        currentIndex = parent(currentIndex);
    }
    
    return true;
}

// 递归计算节点下所有子节点的数量（包括嵌套子节点）
int TreeModel::calculateTotalChildCount(TreeItem* item) const {
    if (!item)
        return 0;
    
    int count = 0;
    for (int i = 0; i < item->childCount(); ++i) {
        // 添加直接子节点
        count++;
        // 递归添加孙子节点
        count += calculateTotalChildCount(item->child(i));
    }
    return count;
}

// 获取节点的完整路径（从根节点到当前节点）
QStringList TreeModel::getNodePath(const QModelIndex &index) const {
    QStringList path;
    
    if (!index.isValid())
        return path;
    
    // 从当前节点向上遍历，收集所有父节点的名称
    QModelIndex currentIndex = index;
    while (currentIndex.isValid()) {
        QString nodeName = data(currentIndex, Qt::DisplayRole).toString();
        path.prepend(nodeName);
        currentIndex = parent(currentIndex);
    }
    
    return path;
}

// 获取节点的深度（层级）
int TreeModel::getNodeDepth(const QModelIndex &index) const {
    int depth = 0;
    QModelIndex parentIndex = parent(index);
    
    while (parentIndex.isValid()) {
        depth++;
        parentIndex = parent(parentIndex);
    }
    
    return depth;
}

// 根据节点类型获取适当的图标URL
QUrl TreeModel::getIconForNode(const QModelIndex &index) const {
    // 获取节点的深度（层级）
    int depth = getNodeDepth(index);
    
    // 获取节点显示的文本
    QString nodeName = data(index, Qt::DisplayRole).toString();
    
    // 根据深度和名称选择适当的图标
    if (depth == 0) {
        // 可见根节点 - 系统图标
        return QUrl("qrc:/icons/system.png");
    } else if (depth == 1) {
        // 类别节点 - 根据类别名称选择不同图标
        if (nodeName == "泛型") return QUrl("qrc:/icons/generic.png");
        if (nodeName == "水体") return QUrl("qrc:/icons/water.png");
        if (nodeName == "电讯") return QUrl("qrc:/icons/telecom.png");
        if (nodeName == "管线") return QUrl("qrc:/icons/pipeline.png");
        if (nodeName == "气体") return QUrl("qrc:/icons/gas.png");
        
        // 默认类别图标
        return QUrl("qrc:/icons/folder.png");
    } else {
        // 特征绘制类型节点 - 根据类型选择不同图标
        if (nodeName == "Pipe") return QUrl("qrc:/icons/pipe.png");
        if (nodeName == "Point") return QUrl("qrc:/icons/point.png");
        if (nodeName == "Area") return QUrl("qrc:/icons/area.png");
        
        // 默认特征类型图标
        return QUrl("qrc:/icons/feature.png");
    }
    
    // 如果都不匹配，返回默认图标
    return QUrl("qrc:/icons/default.png");
}
