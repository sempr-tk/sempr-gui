#ifndef SEMPR_GUI_GEOSQCOORDINATETRANSFORM_HPP_
#define SEMPR_GUI_GEOSQCOORDINATETRANSFORM_HPP_

#include <geos/geom/CoordinateSequenceFilter.h>
#include <QGeoCoordinate>
#include <QList>
#include <QVariant>

namespace sempr { namespace gui {

/**
    A coordinate filter that reads the coordinate sequence of a geometry
    and converts it to a QList<QVariant>, where the variants hold
    QGeoCoordinates.
*/
class ReadCoordinates : public geos::geom::CoordinateSequenceFilter {
    QList<QVariant> coordinates_;
    bool done_;
protected:
    void read(const geos::geom::CoordinateSequence&);
public:
    ReadCoordinates();

    void filter_rw(geos::geom::CoordinateSequence&, size_t index) override;
    void filter_ro(const geos::geom::CoordinateSequence&, size_t index) override;
    bool isDone() const override;
    bool isGeometryChanged() const override;

    /**
        Returns the list of coordinates.
    */
    QList<QVariant> coordinates() const;

    /**
        Resets the internal state in order to reuse the filter.
    */
    void reset();
};


/**
    A coordinate filter that replaces the coordinate sequence of a geometry
    with the given one.
*/
class WriteCoordinates : public geos::geom::CoordinateSequenceFilter {
    QList<QGeoCoordinate> coordinates_;
    bool done_;
public:
    WriteCoordinates(const QList<QGeoCoordinate>& coordinates);

    void filter_rw(geos::geom::CoordinateSequence&, size_t index) override;
    void filter_ro(const geos::geom::CoordinateSequence&, size_t index) override;
    bool isDone() const override;
    bool isGeometryChanged() const override;
};


}}

#endif /* include guard: SEMPR_GUI_GEOSQCOORDINATETRANSFORM_HPP_ */
