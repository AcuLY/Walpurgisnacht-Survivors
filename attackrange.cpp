#include "attackrange.h"

EllipseRange::EllipseRange(double a, double b) : a(a), b(b) {
}

QPainterPath EllipseRange::createPath(const QPointF &center) const {
    QPainterPath path;
    path.addEllipse(center.x() - a, center.y() - b, a * 2, b * 2);
    return path;
}

QPainterPath EllipseRange::createTrack(const QPointF &center1, const QPointF &center2) const {
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

    double dx = center2.x() - center1.x();
    double dy = center2.y() - center1.y();
    double distance = MathUtils::euclideanDistance(center1, center2);
    double scale = a / distance;

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
