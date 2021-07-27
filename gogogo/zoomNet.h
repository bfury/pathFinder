#ifndef GRAPHICS_VIEW_ZOOM_H
#define GRAPHICS_VIEW_ZOOM_H

#include <QObject>
#include <QGraphicsView>

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

class Graphics_view_zoom : public QObject {
  Q_OBJECT
public:
  Graphics_view_zoom(QGraphicsView* view);
  void gentle_zoom(double factor);
  void set_modifiers(Qt::KeyboardModifiers modifiers);
  void set_zoom_factor_base(double value);

private:
  QGraphicsView* _view;
  Qt::KeyboardModifiers _modifiers;
  double _zoom_factor_base;
  QPointF target_scene_pos, target_viewport_pos;
  bool eventFilter(QObject* object, QEvent* event);

signals:
  void zoomed();
};

#endif // GRAPHICS_VIEW_ZOOM_H
