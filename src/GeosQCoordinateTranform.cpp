#include "GeosQCoordinateTranform.hpp"

#include <geos/geom/CoordinateSequence.h>
#include <QGeoCoordinate>

namespace sempr { namespace gui {


// -------------------------------------------------------------------------
// ReadCoordinates
// -------------------------------------------------------------------------
ReadCoordinates::ReadCoordinates()
    : done_(false)
{
}

void ReadCoordinates::read(const geos::geom::CoordinateSequence& sequence)
{
    for (size_t i = 0; i < sequence.size(); i++)
    {
        auto& coordinate = sequence.getAt(i);
        coordinates_.push_back(
            QVariant::fromValue(
                //                  lat         long        alt
                QGeoCoordinate(coordinate.y, coordinate.x, coordinate.z)
            )
        );
    }

    done_ = true;
}

void ReadCoordinates::filter_rw(geos::geom::CoordinateSequence& sequence, size_t)
{
    read(sequence);
}

void ReadCoordinates::filter_ro(const geos::geom::CoordinateSequence& sequence, size_t)
{
    read(sequence);
}

bool ReadCoordinates::isDone() const
{
    return done_;
}

bool ReadCoordinates::isGeometryChanged() const
{
    return false;
}

QList<QVariant> ReadCoordinates::coordinates() const
{
    return coordinates_;
}

void ReadCoordinates::reset()
{
    coordinates_.clear();
    done_ = false;
}

// -------------------------------------------------------------------------
// WriteCoordinates
// -------------------------------------------------------------------------
WriteCoordinates::WriteCoordinates(const QList<QGeoCoordinate>& coordinates)
    : coordinates_(coordinates), done_(false)
{
}

void WriteCoordinates::filter_ro(const geos::geom::CoordinateSequence&, size_t)
{
    // wtf are you doing?!
    throw std::exception();
}

void WriteCoordinates::filter_rw(geos::geom::CoordinateSequence& sequence, size_t)
{
    std::vector<geos::geom::Coordinate> points;
    for (auto& qcoord : coordinates_)
    {
        geos::geom::Coordinate coord(qcoord.longitude(), qcoord.latitude(), qcoord.altitude());
        points.push_back(coord);
    }

    sequence.setPoints(points);
    done_ = true;
}

bool WriteCoordinates::isDone() const
{
    return done_;
}

bool WriteCoordinates::isGeometryChanged() const
{
    return true;
}


}}
