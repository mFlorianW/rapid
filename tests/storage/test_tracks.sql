-- INSERT OSCHERSLEBEN TRACK
INSERT INTO Position (Latitude, Longitude) VALUES
   (52.0270889, 11.2803483), -- Finishline Position
   (52.0271, 11.2804), -- Startline Position
   (52.0298205, 11.2741851), -- Sektor1 Position
   (52.0299681, 11.2772076); -- Sektor2 Position

-- INSERT INTO TRACKS (Longitude, Latitude)
INSERT INTO Track (Name, Finishline, Startline) VALUES
    ('Oschersleben', (SELECT PositionId FROM Position WHERE Latitude = 52.0270889 AND Longitude = 11.2803483), (SELECT PositionId FROM Position WHERE Latitude = 52.0271 AND Longitude = 11.2804));

-- SECTORS
-- SEKTOR1
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE name = 'Oschersleben'), (SELECT PositionId FROM Position WHERE Latitude = 52.0298205 AND Longitude = 11.2741851), 1);

-- SEKTOR2
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE NAME = 'Oschersleben'), (SELECT PositionId FROM Position WHERE Latitude = 52.0299681 AND Longitude = 11.2772076), 2);

-- INSERT ASSEN
INSERT INTO Position (Latitude, Longitude) VALUES
    (52.962324, 6.524115), -- Finishline
    (52.959453, 6.525305), -- Sektor1 Position
    (52.955628, 6.512773); -- Sekotr2 Position

-- INSERT INTO TRACKS (Longitude, Latitude)
INSERT INTO Track (Name, Finishline) VALUES
    ('Assen', (SELECT PositionId FROM Position WHERE Latitude = 52.962324 AND Longitude = 6.524115));

-- SECTORS
-- SEKTOR1
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE name = 'Assen'), (SELECT PositionId FROM Position WHERE Latitude = 52.959453 AND Longitude = 6.525305), 1);

-- SEKTOR2
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE NAME = 'Assen'), (SELECT PositionId FROM Position WHERE Latitude = 52.955628 AND Longitude = 6.512773), 2);
