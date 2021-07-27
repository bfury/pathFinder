#include "zoomNet.h"
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>

/*！
 *此类增加了使用鼠标滚轮缩放QGraphicsView的功能。光标下的点
 *在可能的情况下保持静止。
 *
 *请注意，当场景的
 *尺寸与视口尺寸相比还不够大。 QGraphicsView将图片居中
 *当它小于视图时。而且QGraphicsView的滚动边界不允许
 *将任何图片点放置在任何视口位置。
 *
 *当用户开始滚动时，此类会记住原始场景位置并
 *保留它直到滚动完成。比获得原始场景位置更好
 *每个滚动步骤，因为该方法由于上述原因导致位置错误
 *定位限制。
 *
 *使用滚动放大时，此类发出zoomed（）信号。
 *
 *用法：
 *
 * new Graphics_view_zoom（view）;
 *
 *删除视图时，对象将被自动删除。
 *
 *您可以使用set_modified（）设置用于缩放的键盘修饰符。缩放将
 *仅在修饰符组合完全匹配时执行。默认修饰符为Ctrl。
 *
 *您可以通过调用set_zoom_factor_base（）来更改缩放速度。
 *缩放系数计算为zoom_factor_base ^ angle_delta
 *（请参阅QWheelEvent :: angleDelta）。
 *默认缩放系数基数为_zoom_factor_base。
 */
Graphics_view_zoom::Graphics_view_zoom(QGraphicsView* view)
  : QObject(view), _view(view)
{
  _view->viewport()->installEventFilter(this);
  _view->setMouseTracking(true);
  _modifiers = Qt::ControlModifier;
  _zoom_factor_base = 1.005;
}

void Graphics_view_zoom::gentle_zoom(double factor) {
  _view->scale(factor, factor);
  _view->centerOn(target_scene_pos);
  QPointF delta_viewport_pos = target_viewport_pos - QPointF(_view->viewport()->width() / 2.0,
                                                             _view->viewport()->height() / 2.0);
  QPointF viewport_center = _view->mapFromScene(target_scene_pos) - delta_viewport_pos;
  _view->centerOn(_view->mapToScene(viewport_center.toPoint()));
  emit zoomed();
}

void Graphics_view_zoom::set_modifiers(Qt::KeyboardModifiers modifiers) {
  _modifiers = modifiers;

}

void Graphics_view_zoom::set_zoom_factor_base(double value) {
  _zoom_factor_base = value;
}

bool Graphics_view_zoom::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
    QPointF delta = target_viewport_pos - mouse_event->pos();
    if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
      target_viewport_pos = mouse_event->pos();
      target_scene_pos = _view->mapToScene(mouse_event->pos());
    }
  } else if (event->type() == QEvent::Wheel) {
    QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
    if (QApplication::keyboardModifiers() == _modifiers) {
      if (wheel_event->orientation() == Qt::Vertical) {
        double angle = wheel_event->angleDelta().y();
        double factor = qPow(_zoom_factor_base, angle);
        gentle_zoom(factor);
        return true;
      }
    }
  }
  Q_UNUSED(object)
  return false;
}
