-- INSERT OSCHERSLEBEN TRACK
INSERT INTO Position (Latitude, Longitude) VALUES
   (52.0258333, 11.279166666), -- Finishline Position
   (52.0258333, 11.279166666), -- Startline Position
   (52.0258333, 11.279166666), -- Sektor1 Position
   (52.258335, 11.279166666); -- Sektor2 Position

-- INSERT INTO TRACKS (Longitude, Latitude)
INSERT INTO Track (Name, Finishline, Startline) VALUES
    ('Oschersleben', (SELECT PositionId FROM Position WHERE Latitude = 52.0258333 AND Longitude = 11.279166666), (SELECT PositionId FROM Position WHERE Latitude = 52.0258333 AND Longitude = 11.279166666));

-- SECTORS
-- SEKTOR1
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE name = 'Oschersleben'), (SELECT PositionId FROM Position WHERE Latitude = 52.0258333 AND Longitude = 11.279166666), 1);

-- SEKTOR2
INSERT INTO Sektor (TrackId, PositionId, SektorIndex)
    VALUES
    ((SELECT TrackId FROM Track WHERE NAME = 'Oschersleben'), (SELECT PositionId FROM Position WHERE Latitude = 52.258335 AND Longitude = 11.279166666), 2);
