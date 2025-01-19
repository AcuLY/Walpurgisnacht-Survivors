#include "attackrange.h"
#include "utils.h"

EllipseRange::EllipseRange(double a, double b) : a(a), b(b) {
}

QPainterPath EllipseRange::createPath(const QPointF &center) const {
    QPainterPath path;
    path.addEllipse(center.x() - a, center.y() - b, a * 2, b * 2); // 绘制椭圆
    return path;
}

QPainterPath EllipseRange::createTrack(const QPointF &center1, const QPointF &center2) const {
    // 该函数暂未实现
    QPainterPath track;
    return track;
};

bool EllipseRange::contains(const QPointF &center, const QRectF &target) const {
    QPainterPath range = createPath(center);
    return range.intersects(target);
}

CircleRange::CircleRange(double r) : EllipseRange(r, r) {
}

QPainterPath CircleRange::createTrack(const QPointF &center1, const QPointF &center2) const {
    QPainterPath track;

    // 绘制一个圆形平移后得到的胶囊形
    double dx = center2.x() - center1.x();
    double dy = center2.y() - center1.y();
    double distance = MathUtils::euclideanDistance(center1, center2);
    double scale = a / distance; // a 即半径，scale 即 1 / sqrt(dx + dy)

    QPointF start1 = center1 + QPointF(-scale * dy, scale * dx);
    QPointF control1 = center1 - QPointF(scale * dx, scale * dy);
    QPointF end1 = center1 + QPointF(scale * dy, -scale * dx);

    QPointF start2 = center2 + QPointF(-scale * dy, scale * dx);
    QPointF control2 = center2 + QPointF(scale * dx, scale * dy);
    QPointF end2 = center2 + QPointF(scale * dy, -scale * dx);

    track.moveTo(start1);
    track.cubicTo(control1, end1, end1);
    track.lineTo(end2);
    track.cubicTo(control2, start2, start2);
    track.lineTo(start1);

    return track;
};

SectorRange::SectorRange(double radius, double startAngle, double spanAngle)
    : radius(radius), startAngle(startAngle), spanAngle(spanAngle) {
}

QPainterPath SectorRange::createPath(const QPointF &center) const {
    QPainterPath path;

    // 绘制扇形
    double startAngleDegrees = qRadiansToDegrees(startAngle - M_PI / 6); // 调整到 x 轴正方形
    double spanAngleDegrees = qRadiansToDegrees(spanAngle);

    path.moveTo(center);
    QRect arcRect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
    path.arcTo(arcRect, startAngleDegrees, spanAngleDegrees);

    return path;
}

QPainterPath SectorRange::createTrack(const QPointF &center1, const QPointF &center2) const {
    QPainterPath track = createPath(center2);

    // 绘制扇形平移得到的轨迹
    QPointF startPoint1 = center1 + QPointF(radius * cos(startAngle), radius * sin(startAngle));
    QPointF startPoint2 = center1 - QPointF(radius * cos(startAngle), radius * sin(startAngle));

    track.lineTo(startPoint1);
    track.lineTo(startPoint2);
    track.closeSubpath();

    return track;
}

bool SectorRange::contains(const QPointF &center, const QRectF &target) const {
    QPainterPath range = createPath(center);
    return range.intersects(target);
}
