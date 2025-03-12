-- INSERT OSCHERSLEBEN TRACK
INSERT INTO Position (Latitude, Longitude) VALUES
   (52.0270889, 11.2803483), -- Finishline Position
   (52.0298205, 11.2741851), -- Sektor1 Position
   (52.0299681, 11.2772076); -- Sektor2 Position

-- INSERT INTO TRACKS (Longitude, Latitude)
INSERT INTO Track (Name, Finishline) VALUES
    ('Oschersleben', (SELECT PositionId FROM Position WHERE Latitude = 52.0270889 AND Longitude = 11.2803483));

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

-- INSERT MOST
INSERT INTO Position (Latitude, Longitude) VALUES
    (50.519449, 13.607738), -- Finishline
    (50.515869, 13.591082),
    (50.521772, 13.600161);

-- INSERT INTO TRACKS (Longitude, Latitude)
INSERT INTO Track (Name, Finishline) VALUES
    ('Most', (SELECT PositionId FROM Position WHERE Latitude = 50.519449 AND Longitude = 13.607738));

-- SECTORS
-- SEKTOR1
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE name = 'Most'), (SELECT PositionId FROM Position WHERE Latitude = 50.515869 AND Longitude = 13.591082), 1);

-- SEKTOR2
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE NAME = 'Most'), (SELECT PositionId FROM Position WHERE Latitude = 50.521772 AND Longitude = 13.600161), 2);
