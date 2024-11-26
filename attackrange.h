#ifndef ATTACKRANGE_H
#define ATTACKRANGE_H

#include <QPainterPath>
#include <QPointF>
#include <QRectF>

#include "utils.h"

class AttackRange {
public:
    virtual ~AttackRange() = default;

    virtual bool contains(const QPointF &center, const QRectF &target) const = 0;

    virtual QPainterPath createPath(const QPointF &center) const = 0;
    virtual QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const = 0;
};

class EllipseRange : public AttackRange {
public:
    explicit EllipseRange(double a, double b);
    ~EllipseRange() = default;

    QPainterPath createPath(const QPointF &center) const override;
    QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const override;

    bool contains(const QPointF &center, const QRectF &target) const override;

protected:
    double a, b;
};

class CircleRange : public EllipseRange {
public:
    explicit CircleRange(double r);
    ~CircleRange() = default;

    QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const override;
};

#endif // ATTACKRANGE_H
