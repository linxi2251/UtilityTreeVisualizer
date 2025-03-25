pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import UtilityTreeVisualizer

ApplicationWindow {
  id: root
  width: 640
  height: 480
  visible: true
  title: qsTr("病害标注树状图")

  // 保存当前选中的项
  property var selectedItem: null
  
  // 保存当前项的层级信息
  property string currentHierarchyInfo: ""
  
  // 保存当前选中项的深度
  property int selectedItemDepth: -1
  
  // 添加特征绘制类型的函数
  function addFeatureType(featureType) {
    // 获取当前选中项的索引，如果没有选中项则使用根索引
    var parentIndex = selectedItem || treeModel.getIndex(-1, -1);
    var success = treeModel.addItem(parentIndex, featureType);
    
    if (!success) {
      // 显示错误提示
      errorDialog.open();
    }
  }

  // 错误对话框
  Dialog {
    id: errorDialog
    title: qsTr("操作不允许")
    standardButtons: Dialog.Ok
    anchors.centerIn: Overlay.overlay
    modal: true

    Label {
      text: qsTr("只能在类别节点下添加特征绘制类型，特征绘制类型节点不能再添加子节点。")
    }
  }


  // 删除确认对话框
  Dialog {
    id: deleteConfirmDialog
    title: qsTr("确认删除")
    standardButtons: Dialog.Yes | Dialog.No
    anchors.centerIn: Overlay.overlay
    modal: true

    Label {
      text: qsTr("确定要删除此项吗？")
    }

    onAccepted: {
      if (root.selectedItem) {
        treeModel.removeItem(root.selectedItem);
        root.selectedItem = null;
        // 清空层级信息
        root.currentHierarchyInfo = "";
      }
    }
  }

  TreeModel {
    id: treeModel
  }
  
  // 特征类型弹出菜单 - 移动到ApplicationWindow的直接子元素位置
  Popup {
    id: featureTypePopup
    parent: Overlay.overlay
    anchors.centerIn: parent
    width: 180
    height: 150
    padding: 10
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    
    background: Rectangle {
      color: palette.window
      border.color: palette.mid
      border.width: 1
      radius: 5
    }

    ColumnLayout {
      anchors.fill: parent
      spacing: 8

      Label {
        text: qsTr("选择特征绘制类型:")
        font.bold: true
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
        Layout.bottomMargin: 5
      }

      Button {
        text: qsTr("Pipe 管道")
        Layout.fillWidth: true
        icon.source: "qrc:/icons/pipe.png"
        onClicked: {
          root.addFeatureType("Pipe")
          featureTypePopup.close()
        }
      }

      Button {
        text: qsTr("Point 点")
        Layout.fillWidth: true
        icon.source: "qrc:/icons/point.png"
        onClicked: {
          root.addFeatureType("Point")
          featureTypePopup.close()
        }
      }

      Button {
        text: qsTr("Area 面")
        Layout.fillWidth: true
        icon.source: "qrc:/icons/area.png"
        onClicked: {
          root.addFeatureType("Area")
          featureTypePopup.close()
        }
      }
    }
  }
  
  // 状态栏显示层级信息
  footer:Rectangle {
    Layout.fillWidth: true
    height: 30
    color: palette.base
    visible: root.currentHierarchyInfo !== ""

    Label {
      anchors.fill: parent
      anchors.leftMargin: 10
      verticalAlignment: Text.AlignVCenter
      text: root.currentHierarchyInfo
      elide: Text.ElideRight
    }
  }


  ColumnLayout {
    anchors.fill: parent
    spacing: 10

    // 树视图
    TreeView {
      id: treeView
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: treeModel
      delegate: treeViewDelegateComponent

      selectionModel: ItemSelectionModel {}
      
      // 组件加载完成后展开所有节点
      Component.onCompleted: {
        // 展开可见根节点（索引0）
        /**
          Expands the tree node at the given row in the view recursively down to depth.
          depth should be relative to the depth of row.
          If depth is -1, the tree will be expanded all the way down to all leaves
        */
        treeView.expandRecursively(0, -1)
      }
    }

    // 操作区域
    RowLayout {
      Layout.fillWidth: true
      Layout.margins: 10
      spacing: 10

      Button {
        id: addButton
        text: qsTr("+")
        font.pixelSize: 18
        font.bold: true
        implicitWidth: 40
        implicitHeight: 40
        ToolTip.visible: hovered && root.selectedItemDepth === 1
        ToolTip.text: qsTr("添加特征绘制类型")
        // 只有当选中类别节点（深度为1）时才启用
        enabled: root.selectedItemDepth === 1
        onClicked: {
          featureTypePopup.open()
        }
      }
      Button {
        Layout.fillWidth: true
      }
    }
  }

  Component {
    id: treeViewDelegateComponent

    TreeViewDelegate {
      id: treeViewDelegate
      implicitWidth: treeView.width
      onClicked: {
        console.log("Selected: " + treeViewDelegate.displaying);
        root.selectedItem = treeView.index(row, 0);
        // 更新选中项的深度
        root.selectedItemDepth = treeViewDelegate.depth;
        
        // 使用新的getNodePath方法获取完整路径
        let nodePath = treeModel.getNodePath(root.selectedItem);
        
        if (nodePath.length > 0) {
          // 更新层级信息，显示完整路径
          root.currentHierarchyInfo = nodePath.join(" > ");
          
          // 如果是特征绘制类型（depth=2），记录更详细的信息
          if (treeViewDelegate.depth === 2 && nodePath.length >= 3) {
            let featureType = nodePath[2]; // 特征绘制类型
            let category = nodePath[1];    // 类别
            let rootNode = nodePath[0];    // 根节点
            
            console.log(`当前特征绘制类型 ${featureType} 属于类别: ${category}, 根节点: ${rootNode}`);
          }
        }
      }

      //Defined by roleNames() in backend
      required property string displaying
      required property int childCount
      required property var decoration

      background: Rectangle {
        color: {
          if (treeViewDelegate.current) {
            if (treeViewDelegate.depth === 2) {
              return palette.highlight
            } else {
              return palette.base
            }
          } else {
            return "transparent"
          }
        }
      }

      contentItem: RowLayout {
        spacing: 5
        Image {
          source: treeViewDelegate.decoration
          fillMode: Image.PreserveAspectFit
        }
        
        Label {
          font.pixelSize: 15
          text: treeViewDelegate.displaying
        }
        Item {
          Layout.fillWidth: true
        }
        Label {
          visible: treeViewDelegate.childCount > 0
          font.pixelSize: 12
          color: "gray"
          text: "(" + treeViewDelegate.childCount + ")"
        }

        Button {
          text: "删除"
          Layout.preferredWidth: 32
          visible: treeViewDelegate.childCount == 0
          onClicked: {
            // 更新当前selectedItem以删除当前项
            root.selectedItem = treeView.index(treeViewDelegate.row, 0);
            // 显示确认对话框
            deleteConfirmDialog.open();
          }
        }
        Item {
          Layout.preferredWidth: 10
        }
      }
    }
  }
}


