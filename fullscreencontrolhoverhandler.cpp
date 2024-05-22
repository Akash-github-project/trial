#include "fullscreencontrolhoverhandler.h"

FullScreenControlHoverHandler::FullScreenControlHoverHandler(QObject *parent)
    : QGraphicsRectItem(nullptr), controlledWidget(nullptr) {
    setAcceptHoverEvents(true);
}
