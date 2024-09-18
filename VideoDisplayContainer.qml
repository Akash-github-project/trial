import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15

item {
    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        source: video
        fillMode: VideoOutput.PreserveAspectFit

        Video {
          id: video
           // The source will be set from C++ dynamically
           // Make sure that the source property is updated from C++
           //autoPlay: true
        }
    }
}
