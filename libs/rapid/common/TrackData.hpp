// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "PositionData.hpp"
#include "SharedDataPointer.hpp"
#include <string>
#include <vector>

namespace Rapid::Common
{

class SharedTrackData;

/**
 * The track data class contains the data of a real racetrack.
 */
class TrackData final
{
public:
    /**
     * Creates an instance of TrackData
     */
    TrackData();

    /**
     * Default destructor.
     */
    ~TrackData();

    /**
     * Copy constructor for TrackData
     * @param ohter The object to copy from.
     */
    TrackData(TrackData const& ohter);

    /**
     * The copy assignment operator for TrackData.
     * @param other The object to copy from.
     * @return TrackData& A reference to the copied track.
     */
    TrackData& operator=(TrackData const& other);

    /**
     * Move constructor for TrackData
     * @param other The object to move from.
     */
    TrackData(TrackData&& other);

    /**
     * The move assignment operator for the TrackData.
     * @param other The object to move from.
     * @return TrackData& A reference to the moved track data.
     */
    TrackData& operator=(TrackData&& other);

    /**
     * Gives the track name.
     * @return const std::string& The track name.
     */
    std::string const& getTrackName() const;

    /**
     * Sets the new track name.
     * @param name The new name of the track.
     */
    void setTrackName(std::string const& name);

    /**
     * Gives the position data of the start line.
     * @return const PositionData& The position of the start line.
     */
    PositionData const& getStartline() const;

    /**
     * Set a new position for the start line.
     * @param startLine The new position for the start line.
     */
    void setStartline(PositionData const& startLine);

    /**
     * Gives the position of the finish line.
     * @return const PositionData&
     */
    PositionData const& getFinishline() const;

    /**
     * Sets a new position for the finish line.
     * @param finishLine The new position of the finish line.
     */
    void setFinishline(PositionData const& finishline);

    /**
     * Gives the number of sections of the race track.
     * @return size_t The number of sections.
     */
    size_t getNumberOfSections() const;

    /**
     * Gives the position of a section given by the index.
     * If the index is bigger than the section count a default constructed PositionData will be returned.
     * @param sectionIndex The section index.
     * @return const PositionData& The position data of the section.
     */
    PositionData const& getSection(size_t sectionIndex) const;

    /**
     * Gives a list of all sections of the track.
     * @return const std::vector<PositionData>& A list with all sections of the track.
     */
    std::vector<PositionData> const& getSections() const;

    /**
     * Sets the sections for the race track.
     * The sections are consumed and the passed vector is not longer available after it.
     * @param sections The new sections of the track.
     */
    void setSections(std::vector<PositionData> const& sections);

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(TrackData const& lhs, TrackData const& rhs);

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(TrackData const& lhs, TrackData const& rhs);

private:
    SharedDataPointer<SharedTrackData> mData;
};

} // namespace Rapid::Common
