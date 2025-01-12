// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LAP_HPP
#define LAP_HPP

#include "GpsPositionData.hpp"
#include "SharedDataPointer.hpp"
#include "Timestamp.hpp"
#include <optional>
#include <vector>

namespace Rapid::Common
{
class SharedLap;

/**
 * A lap is representation of lap and is part of session.
 * The lap has a laptime and depending on the track amount sector times.
 */
class LapData final
{
public:
    /**
     * Creates an empty lap
     */
    LapData();

    /**
     * Creates an instance of lapdata
     * @param laptime The time which is used a laptime.
     */
    explicit LapData(Timestamp const& laptime);

    /**
     * Constructs LapData instance with
     * @param sectorTimes The array of sector times.
     */
    explicit LapData(std::vector<Timestamp> const& sectorTimes);

    /**
     * Default destructor
     */
    ~LapData();

    /**
     * Copy constructor for LapData
     * @param ohter The object to copy from.
     */
    LapData(LapData const& ohter);

    /**
     * The copy assignment operator for LapData.
     * @param other The object to copy from.
     * @return LapData& A reference to the copied track.
     */
    LapData& operator=(LapData const& other);

    /**
     * Move constructor for LapData
     * @param other The object to move from.
     */
    LapData(LapData&& other) noexcept;

    /**
     * The move assignment operator for the LapData.
     * @param other The object to move from.
     * @return LapData& A reference to the moved track data.
     */
    LapData& operator=(LapData&& other) noexcept;

    /**
     * Gives the overall laptime.
     * The laptime is the accmulation overall sector times and is only valid
     * if all sector of the lap are added.
     * @return The overall laptime.
     */
    [[nodiscard]] Timestamp getLaptime() const noexcept;

    /**
     * Gives the amount of sectors.
     * @return The number of sectors.
     */
    [[nodiscard]] std::size_t getSectorTimeCount() const noexcept;

    /**
     * Gives a sector time for a specific sector.
     * If the index is bigger than the lap sector count a nullopt is returned.
     * @param index The index of the sector.
     * @return The sector time for the index.
     */
    [[nodiscard]] std::optional<Timestamp> getSectorTime(std::size_t index) const noexcept;

    /**
     * Gives a list of all sector times.
     * @return A list with all sector times.
     */
    [[nodiscard]] std::vector<Timestamp> const& getSectorTimes() const noexcept;

    /**
     * Gives the stored positions information for that lap.
     * The position in the list is the order of the data.
     * return The stored position of that lap.
     */
    [[nodiscard]] std::vector<GpsPositionData> const& getPositions() const noexcept;

    /**
     * Adds a new sector time to the lap.
     * The order to this function calls define the sector ordering.
     * @param sectorTime The sector that shall be added.
     */
    void addSectorTime(Timestamp const& sectorTime);

    /**
     * Adds a list of sector times to the lap.
     * The order of the list defines the ordering of the sectors.
     * @param sectorTimes The list of sector times.
     */
    void addSectorTimes(std::vector<Timestamp> const& sectorTimes);

    /**
     * Adds a position to the list of positions of the lap.
     * @param pos The position that shall be added.
     */
    void addPosition(GpsPositionData const& pos);

    /**
     * Overwrite all position data for the Lap.
     * @param positions The new position data for that lap.
     */
    void overwritePositions(std::vector<GpsPositionData> const& positions);

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(LapData const& lhs, LapData const& rhs);

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(LapData const& lhs, LapData const& rhs);

private:
    SharedDataPointer<SharedLap> mData;
};

} // namespace Rapid::Common
#endif // LAP_HPP
