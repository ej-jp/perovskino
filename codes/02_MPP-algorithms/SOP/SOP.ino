#include <Wire.h>
#include <math.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_INA219.h>
//#include <LTR390.h>
//#include <DHT.h>

//Solar cell testing
Adafruit_MCP4725 dac;
Adafruit_INA219 ina219(0x40);

//Thermistors settings
//Sourcingmap – a14092200ux0209 10 K Ohm 0,05 W
//NTC Thermistor;Model : MF52-103;Insulation Material : Ceramic
//Color : Black;Rated Power : 0.05W;Resistance Value : 10K Ohm
//B Value : 3435(+/-1%);Pin Pitch : 1.5mm / 0.059""
const int Rc = 10000; //resistance value of the voltage divider
const float Vcc = 5.0;//input voltage of the voltage divider (arduino)
const int SensorPIN = A0;//voltage readings for thermistor in solar cell testing
//Thermistor parameters
#define A 1.098E-03
#define  B 2.446E-04
#define  C -3.693E-09


//Forward fast voltage scan
const int dac_steps1 = 101;
const PROGMEM int DACLookup_list1[dac_steps1] =
{
  1775, 1770, 1765, 1760, 1755, 1750, 1745, 1740, 1735, 1730, 1725, 1720, 1715, 1710, 1705, 1700, 1695, 1690, 1685, 1680, 1675, 1670, 1665, 1660, 1655, 1650, 1645, 1640, 1635, 1630, 1625, 1620, 1615, 1610, 1605, 1600, 1595, 1590, 1585, 1580, 1575, 1570, 1565, 1560, 1555, 1550, 1545, 1540, 1535, 1530, 1525, 1520, 1515, 1510, 1505, 1500, 1495, 1490, 1485, 1480, 1475, 1470, 1465, 1460, 1455, 1450, 1445, 1440, 1435, 1430, 1425, 1420, 1415, 1410, 1405, 1400, 1395, 1390, 1385, 1380, 1375, 1370, 1365, 1360, 1355, 1350, 1345, 1340, 1335, 1330, 1325, 1320, 1315, 1310, 1305, 1300, 1295, 1290, 1285, 1280, 1275
};
//Backward fast voltage scan
const int dac_steps2 = 101;
const PROGMEM int DACLookup_list2[dac_steps2] =
{
  1275, 1280, 1285, 1290, 1295, 1300, 1305, 1310, 1315, 1320, 1325, 1330, 1335, 1340, 1345, 1350, 1355, 1360, 1365, 1370, 1375, 1380, 1385, 1390, 1395, 1400, 1405, 1410, 1415, 1420, 1425, 1430, 1435, 1440, 1445, 1450, 1455, 1460, 1465, 1470, 1475, 1480, 1485, 1490, 1495, 1500, 1505, 1510, 1515, 1520, 1525, 1530, 1535, 1540, 1545, 1550, 1555, 1560, 1565, 1570, 1575, 1580, 1585, 1590, 1595, 1600, 1605, 1610, 1615, 1620, 1625, 1630, 1635, 1640, 1645, 1650, 1655, 1660, 1665, 1670, 1675, 1680, 1685, 1690, 1695, 1700, 1705, 1710, 1715, 1720, 1725, 1730, 1735, 1740, 1745, 1750, 1755, 1760, 1765, 1770, 1775
};

//Forward slow voltage scan max resolution
const int dac_steps3 = 501;
const PROGMEM int DACLookup_list3[dac_steps3] =
{
  1775, 1774, 1773, 1772, 1771, 1770, 1769, 1768, 1767, 1766, 1765, 1764, 1763, 1762, 1761, 1760, 1759, 1758, 1757, 1756, 1755, 1754, 1753, 1752, 1751, 1750, 1749, 1748, 1747, 1746, 1745, 1744, 1743, 1742, 1741, 1740, 1739, 1738, 1737, 1736, 1735, 1734, 1733, 1732, 1731, 1730, 1729, 1728, 1727, 1726, 1725, 1724, 1723, 1722, 1721, 1720, 1719, 1718, 1717, 1716, 1715, 1714, 1713, 1712, 1711, 1710, 1709, 1708, 1707, 1706, 1705, 1704, 1703, 1702, 1701, 1700, 1699, 1698, 1697, 1696, 1695, 1694, 1693, 1692, 1691, 1690, 1689, 1688, 1687, 1686, 1685, 1684, 1683, 1682, 1681, 1680, 1679, 1678, 1677, 1676, 1675, 1674, 1673, 1672, 1671, 1670, 1669, 1668, 1667, 1666, 1665, 1664, 1663, 1662, 1661, 1660, 1659, 1658, 1657, 1656, 1655, 1654, 1653, 1652, 1651, 1650, 1649, 1648, 1647, 1646, 1645, 1644, 1643, 1642, 1641, 1640, 1639, 1638, 1637, 1636, 1635, 1634, 1633, 1632, 1631, 1630, 1629, 1628, 1627, 1626, 1625, 1624, 1623, 1622, 1621, 1620, 1619, 1618, 1617, 1616, 1615, 1614, 1613, 1612, 1611, 1610, 1609, 1608, 1607, 1606, 1605, 1604, 1603, 1602, 1601, 1600, 1599, 1598, 1597, 1596, 1595, 1594, 1593, 1592, 1591, 1590, 1589, 1588, 1587, 1586, 1585, 1584, 1583, 1582, 1581, 1580, 1579, 1578, 1577, 1576, 1575, 1574, 1573, 1572, 1571, 1570, 1569, 1568, 1567, 1566, 1565, 1564, 1563, 1562, 1561, 1560, 1559, 1558, 1557, 1556, 1555, 1554, 1553, 1552, 1551, 1550, 1549, 1548, 1547, 1546, 1545, 1544, 1543, 1542, 1541, 1540, 1539, 1538, 1537, 1536, 1535, 1534, 1533, 1532, 1531, 1530, 1529, 1528, 1527, 1526, 1525, 1524, 1523, 1522, 1521, 1520, 1519, 1518, 1517, 1516, 1515, 1514, 1513, 1512, 1511, 1510, 1509, 1508, 1507, 1506, 1505, 1504, 1503, 1502, 1501, 1500, 1499, 1498, 1497, 1496, 1495, 1494, 1493, 1492, 1491, 1490, 1489, 1488, 1487, 1486, 1485, 1484, 1483, 1482, 1481, 1480, 1479, 1478, 1477, 1476, 1475, 1474, 1473, 1472, 1471, 1470, 1469, 1468, 1467, 1466, 1465, 1464, 1463, 1462, 1461, 1460, 1459, 1458, 1457, 1456, 1455, 1454, 1453, 1452, 1451, 1450, 1449, 1448, 1447, 1446, 1445, 1444, 1443, 1442, 1441, 1440, 1439, 1438, 1437, 1436, 1435, 1434, 1433, 1432, 1431, 1430, 1429, 1428, 1427, 1426, 1425, 1424, 1423, 1422, 1421, 1420, 1419, 1418, 1417, 1416, 1415, 1414, 1413, 1412, 1411, 1410, 1409, 1408, 1407, 1406, 1405, 1404, 1403, 1402, 1401, 1400, 1399, 1398, 1397, 1396, 1395, 1394, 1393, 1392, 1391, 1390, 1389, 1388, 1387, 1386, 1385, 1384, 1383, 1382, 1381, 1380, 1379, 1378, 1377, 1376, 1375, 1374, 1373, 1372, 1371, 1370, 1369, 1368, 1367, 1366, 1365, 1364, 1363, 1362, 1361, 1360, 1359, 1358, 1357, 1356, 1355, 1354, 1353, 1352, 1351, 1350, 1349, 1348, 1347, 1346, 1345, 1344, 1343, 1342, 1341, 1340, 1339, 1338, 1337, 1336, 1335, 1334, 1333, 1332, 1331, 1330, 1329, 1328, 1327, 1326, 1325, 1324, 1323, 1322, 1321, 1320, 1319, 1318, 1317, 1316, 1315, 1314, 1313, 1312, 1311, 1310, 1309, 1308, 1307, 1306, 1305, 1304, 1303, 1302, 1301, 1300, 1299, 1298, 1297, 1296, 1295, 1294, 1293, 1292, 1291, 1290, 1289, 1288, 1287, 1286, 1285, 1284, 1283, 1282, 1281, 1280, 1279, 1278, 1277, 1276, 1275
};

//Backward slow voltage scan max resolution
const int dac_steps4 = 501;
const PROGMEM int DACLookup_list4[dac_steps4] =
{
  1275, 1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355, 1356, 1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1406, 1407, 1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1442, 1443, 1444, 1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1455, 1456, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1466, 1467, 1468, 1469, 1470, 1471, 1472, 1473, 1474, 1475, 1476, 1477, 1478, 1479, 1480, 1481, 1482, 1483, 1484, 1485, 1486, 1487, 1488, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, 1512, 1513, 1514, 1515, 1516, 1517, 1518, 1519, 1520, 1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1529, 1530, 1531, 1532, 1533, 1534, 1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 1546, 1547, 1548, 1549, 1550, 1551, 1552, 1553, 1554, 1555, 1556, 1557, 1558, 1559, 1560, 1561, 1562, 1563, 1564, 1565, 1566, 1567, 1568, 1569, 1570, 1571, 1572, 1573, 1574, 1575, 1576, 1577, 1578, 1579, 1580, 1581, 1582, 1583, 1584, 1585, 1586, 1587, 1588, 1589, 1590, 1591, 1592, 1593, 1594, 1595, 1596, 1597, 1598, 1599, 1600, 1601, 1602, 1603, 1604, 1605, 1606, 1607, 1608, 1609, 1610, 1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 1620, 1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648, 1649, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662, 1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679, 1680, 1681, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1690, 1691, 1692, 1693, 1694, 1695, 1696, 1697, 1698, 1699, 1700, 1701, 1702, 1703, 1704, 1705, 1706, 1707, 1708, 1709, 1710, 1711, 1712, 1713, 1714, 1715, 1716, 1717, 1718, 1719, 1720, 1721, 1722, 1723, 1724, 1725, 1726, 1727, 1728, 1729, 1730, 1731, 1732, 1733, 1734, 1735, 1736, 1737, 1738, 1739, 1740, 1741, 1742, 1743, 1744, 1745, 1746, 1747, 1748, 1749, 1750, 1751, 1752, 1753, 1754, 1755, 1756, 1757, 1758, 1759, 1760, 1761, 1762, 1763, 1764, 1765, 1766, 1767, 1768, 1769, 1770, 1771, 1772, 1773, 1774, 1775
};

//Forward-Backward fast voltage scan
const int dac_steps5 = 202;
const PROGMEM int DACLookup_list5[dac_steps5] =
{
  1775, 1770, 1765, 1760, 1755, 1750, 1745, 1740, 1735, 1730, 1725, 1720, 1715, 1710, 1705, 1700, 1695, 1690, 1685, 1680, 1675, 1670, 1665, 1660, 1655, 1650, 1645, 1640, 1635, 1630, 1625, 1620, 1615, 1610, 1605, 1600, 1595, 1590, 1585, 1580, 1575, 1570, 1565, 1560, 1555, 1550, 1545, 1540, 1535, 1530, 1525, 1520, 1515, 1510, 1505, 1500, 1495, 1490, 1485, 1480, 1475, 1470, 1465, 1460, 1455, 1450, 1445, 1440, 1435, 1430, 1425, 1420, 1415, 1410, 1405, 1400, 1395, 1390, 1385, 1380, 1375, 1370, 1365, 1360, 1355, 1350, 1345, 1340, 1335, 1330, 1325, 1320, 1315, 1310, 1305, 1300, 1295, 1290, 1285, 1280, 1275, 1275, 1280, 1285, 1290, 1295, 1300, 1305, 1310, 1315, 1320, 1325, 1330, 1335, 1340, 1345, 1350, 1355, 1360, 1365, 1370, 1375, 1380, 1385, 1390, 1395, 1400, 1405, 1410, 1415, 1420, 1425, 1430, 1435, 1440, 1445, 1450, 1455, 1460, 1465, 1470, 1475, 1480, 1485, 1490, 1495, 1500, 1505, 1510, 1515, 1520, 1525, 1530, 1535, 1540, 1545, 1550, 1555, 1560, 1565, 1570, 1575, 1580, 1585, 1590, 1595, 1600, 1605, 1610, 1615, 1620, 1625, 1630, 1635, 1640, 1645, 1650, 1655, 1660, 1665, 1670, 1675, 1680, 1685, 1690, 1695, 1700, 1705, 1710, 1715, 1720, 1725, 1730, 1735, 1740, 1745, 1750, 1755, 1760, 1765, 1770, 1775
};

//Backward-Forward fast voltage scan
const int dac_steps6 = 202;
const PROGMEM int DACLookup_list6[dac_steps6] =
{
  1275, 1280, 1285, 1290, 1295, 1300, 1305, 1310, 1315, 1320, 1325, 1330, 1335, 1340, 1345, 1350, 1355, 1360, 1365, 1370, 1375, 1380, 1385, 1390, 1395, 1400, 1405, 1410, 1415, 1420, 1425, 1430, 1435, 1440, 1445, 1450, 1455, 1460, 1465, 1470, 1475, 1480, 1485, 1490, 1495, 1500, 1505, 1510, 1515, 1520, 1525, 1530, 1535, 1540, 1545, 1550, 1555, 1560, 1565, 1570, 1575, 1580, 1585, 1590, 1595, 1600, 1605, 1610, 1615, 1620, 1625, 1630, 1635, 1640, 1645, 1650, 1655, 1660, 1665, 1670, 1675, 1680, 1685, 1690, 1695, 1700, 1705, 1710, 1715, 1720, 1725, 1730, 1735, 1740, 1745, 1750, 1755, 1760, 1765, 1770, 1775, 1775, 1770, 1765, 1760, 1755, 1750, 1745, 1740, 1735, 1730, 1725, 1720, 1715, 1710, 1705, 1700, 1695, 1690, 1685, 1680, 1675, 1670, 1665, 1660, 1655, 1650, 1645, 1640, 1635, 1630, 1625, 1620, 1615, 1610, 1605, 1600, 1595, 1590, 1585, 1580, 1575, 1570, 1565, 1560, 1555, 1550, 1545, 1540, 1535, 1530, 1525, 1520, 1515, 1510, 1505, 1500, 1495, 1490, 1485, 1480, 1475, 1470, 1465, 1460, 1455, 1450, 1445, 1440, 1435, 1430, 1425, 1420, 1415, 1410, 1405, 1400, 1395, 1390, 1385, 1380, 1375, 1370, 1365, 1360, 1355, 1350, 1345, 1340, 1335, 1330, 1325, 1320, 1315, 1310, 1305, 1300, 1295, 1290, 1285, 1280, 1275
};

////Forward-Backward slow voltage scan max resolution
const int dac_steps7 = 1002;
const PROGMEM int DACLookup_list7[dac_steps7] =
{
  1775, 1774, 1773, 1772, 1771, 1770, 1769, 1768, 1767, 1766, 1765, 1764, 1763, 1762, 1761, 1760, 1759, 1758, 1757, 1756, 1755, 1754, 1753, 1752, 1751, 1750, 1749, 1748, 1747, 1746, 1745, 1744, 1743, 1742, 1741, 1740, 1739, 1738, 1737, 1736, 1735, 1734, 1733, 1732, 1731, 1730, 1729, 1728, 1727, 1726, 1725, 1724, 1723, 1722, 1721, 1720, 1719, 1718, 1717, 1716, 1715, 1714, 1713, 1712, 1711, 1710, 1709, 1708, 1707, 1706, 1705, 1704, 1703, 1702, 1701, 1700, 1699, 1698, 1697, 1696, 1695, 1694, 1693, 1692, 1691, 1690, 1689, 1688, 1687, 1686, 1685, 1684, 1683, 1682, 1681, 1680, 1679, 1678, 1677, 1676, 1675, 1674, 1673, 1672, 1671, 1670, 1669, 1668, 1667, 1666, 1665, 1664, 1663, 1662, 1661, 1660, 1659, 1658, 1657, 1656, 1655, 1654, 1653, 1652, 1651, 1650, 1649, 1648, 1647, 1646, 1645, 1644, 1643, 1642, 1641, 1640, 1639, 1638, 1637, 1636, 1635, 1634, 1633, 1632, 1631, 1630, 1629, 1628, 1627, 1626, 1625, 1624, 1623, 1622, 1621, 1620, 1619, 1618, 1617, 1616, 1615, 1614, 1613, 1612, 1611, 1610, 1609, 1608, 1607, 1606, 1605, 1604, 1603, 1602, 1601, 1600, 1599, 1598, 1597, 1596, 1595, 1594, 1593, 1592, 1591, 1590, 1589, 1588, 1587, 1586, 1585, 1584, 1583, 1582, 1581, 1580, 1579, 1578, 1577, 1576, 1575, 1574, 1573, 1572, 1571, 1570, 1569, 1568, 1567, 1566, 1565, 1564, 1563, 1562, 1561, 1560, 1559, 1558, 1557, 1556, 1555, 1554, 1553, 1552, 1551, 1550, 1549, 1548, 1547, 1546, 1545, 1544, 1543, 1542, 1541, 1540, 1539, 1538, 1537, 1536, 1535, 1534, 1533, 1532, 1531, 1530, 1529, 1528, 1527, 1526, 1525, 1524, 1523, 1522, 1521, 1520, 1519, 1518, 1517, 1516, 1515, 1514, 1513, 1512, 1511, 1510, 1509, 1508, 1507, 1506, 1505, 1504, 1503, 1502, 1501, 1500, 1499, 1498, 1497, 1496, 1495, 1494, 1493, 1492, 1491, 1490, 1489, 1488, 1487, 1486, 1485, 1484, 1483, 1482, 1481, 1480, 1479, 1478, 1477, 1476, 1475, 1474, 1473, 1472, 1471, 1470, 1469, 1468, 1467, 1466, 1465, 1464, 1463, 1462, 1461, 1460, 1459, 1458, 1457, 1456, 1455, 1454, 1453, 1452, 1451, 1450, 1449, 1448, 1447, 1446, 1445, 1444, 1443, 1442, 1441, 1440, 1439, 1438, 1437, 1436, 1435, 1434, 1433, 1432, 1431, 1430, 1429, 1428, 1427, 1426, 1425, 1424, 1423, 1422, 1421, 1420, 1419, 1418, 1417, 1416, 1415, 1414, 1413, 1412, 1411, 1410, 1409, 1408, 1407, 1406, 1405, 1404, 1403, 1402, 1401, 1400, 1399, 1398, 1397, 1396, 1395, 1394, 1393, 1392, 1391, 1390, 1389, 1388, 1387, 1386, 1385, 1384, 1383, 1382, 1381, 1380, 1379, 1378, 1377, 1376, 1375, 1374, 1373, 1372, 1371, 1370, 1369, 1368, 1367, 1366, 1365, 1364, 1363, 1362, 1361, 1360, 1359, 1358, 1357, 1356, 1355, 1354, 1353, 1352, 1351, 1350, 1349, 1348, 1347, 1346, 1345, 1344, 1343, 1342, 1341, 1340, 1339, 1338, 1337, 1336, 1335, 1334, 1333, 1332, 1331, 1330, 1329, 1328, 1327, 1326, 1325, 1324, 1323, 1322, 1321, 1320, 1319, 1318, 1317, 1316, 1315, 1314, 1313, 1312, 1311, 1310, 1309, 1308, 1307, 1306, 1305, 1304, 1303, 1302, 1301, 1300, 1299, 1298, 1297, 1296, 1295, 1294, 1293, 1292, 1291, 1290, 1289, 1288, 1287, 1286, 1285, 1284, 1283, 1282, 1281, 1280, 1279, 1278, 1277, 1276, 1275, 1275, 1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355, 1356, 1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1406, 1407, 1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1442, 1443, 1444, 1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1455, 1456, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1466, 1467, 1468, 1469, 1470, 1471, 1472, 1473, 1474, 1475, 1476, 1477, 1478, 1479, 1480, 1481, 1482, 1483, 1484, 1485, 1486, 1487, 1488, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, 1512, 1513, 1514, 1515, 1516, 1517, 1518, 1519, 1520, 1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1529, 1530, 1531, 1532, 1533, 1534, 1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 1546, 1547, 1548, 1549, 1550, 1551, 1552, 1553, 1554, 1555, 1556, 1557, 1558, 1559, 1560, 1561, 1562, 1563, 1564, 1565, 1566, 1567, 1568, 1569, 1570, 1571, 1572, 1573, 1574, 1575, 1576, 1577, 1578, 1579, 1580, 1581, 1582, 1583, 1584, 1585, 1586, 1587, 1588, 1589, 1590, 1591, 1592, 1593, 1594, 1595, 1596, 1597, 1598, 1599, 1600, 1601, 1602, 1603, 1604, 1605, 1606, 1607, 1608, 1609, 1610, 1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 1620, 1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648, 1649, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662, 1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679, 1680, 1681, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1690, 1691, 1692, 1693, 1694, 1695, 1696, 1697, 1698, 1699, 1700, 1701, 1702, 1703, 1704, 1705, 1706, 1707, 1708, 1709, 1710, 1711, 1712, 1713, 1714, 1715, 1716, 1717, 1718, 1719, 1720, 1721, 1722, 1723, 1724, 1725, 1726, 1727, 1728, 1729, 1730, 1731, 1732, 1733, 1734, 1735, 1736, 1737, 1738, 1739, 1740, 1741, 1742, 1743, 1744, 1745, 1746, 1747, 1748, 1749, 1750, 1751, 1752, 1753, 1754, 1755, 1756, 1757, 1758, 1759, 1760, 1761, 1762, 1763, 1764, 1765, 1766, 1767, 1768, 1769, 1770, 1771, 1772, 1773, 1774, 1775
};

//Backward-Forward slow voltage scan max resolution
const int dac_steps8 = 1002;
const PROGMEM int DACLookup_list8[dac_steps8] =
{
  1275, 1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355, 1356, 1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1406, 1407, 1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1442, 1443, 1444, 1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1455, 1456, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1466, 1467, 1468, 1469, 1470, 1471, 1472, 1473, 1474, 1475, 1476, 1477, 1478, 1479, 1480, 1481, 1482, 1483, 1484, 1485, 1486, 1487, 1488, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, 1512, 1513, 1514, 1515, 1516, 1517, 1518, 1519, 1520, 1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1529, 1530, 1531, 1532, 1533, 1534, 1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 1546, 1547, 1548, 1549, 1550, 1551, 1552, 1553, 1554, 1555, 1556, 1557, 1558, 1559, 1560, 1561, 1562, 1563, 1564, 1565, 1566, 1567, 1568, 1569, 1570, 1571, 1572, 1573, 1574, 1575, 1576, 1577, 1578, 1579, 1580, 1581, 1582, 1583, 1584, 1585, 1586, 1587, 1588, 1589, 1590, 1591, 1592, 1593, 1594, 1595, 1596, 1597, 1598, 1599, 1600, 1601, 1602, 1603, 1604, 1605, 1606, 1607, 1608, 1609, 1610, 1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 1620, 1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648, 1649, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662, 1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679, 1680, 1681, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1690, 1691, 1692, 1693, 1694, 1695, 1696, 1697, 1698, 1699, 1700, 1701, 1702, 1703, 1704, 1705, 1706, 1707, 1708, 1709, 1710, 1711, 1712, 1713, 1714, 1715, 1716, 1717, 1718, 1719, 1720, 1721, 1722, 1723, 1724, 1725, 1726, 1727, 1728, 1729, 1730, 1731, 1732, 1733, 1734, 1735, 1736, 1737, 1738, 1739, 1740, 1741, 1742, 1743, 1744, 1745, 1746, 1747, 1748, 1749, 1750, 1751, 1752, 1753, 1754, 1755, 1756, 1757, 1758, 1759, 1760, 1761, 1762, 1763, 1764, 1765, 1766, 1767, 1768, 1769, 1770, 1771, 1772, 1773, 1774, 1775, 1775, 1774, 1773, 1772, 1771, 1770, 1769, 1768, 1767, 1766, 1765, 1764, 1763, 1762, 1761, 1760, 1759, 1758, 1757, 1756, 1755, 1754, 1753, 1752, 1751, 1750, 1749, 1748, 1747, 1746, 1745, 1744, 1743, 1742, 1741, 1740, 1739, 1738, 1737, 1736, 1735, 1734, 1733, 1732, 1731, 1730, 1729, 1728, 1727, 1726, 1725, 1724, 1723, 1722, 1721, 1720, 1719, 1718, 1717, 1716, 1715, 1714, 1713, 1712, 1711, 1710, 1709, 1708, 1707, 1706, 1705, 1704, 1703, 1702, 1701, 1700, 1699, 1698, 1697, 1696, 1695, 1694, 1693, 1692, 1691, 1690, 1689, 1688, 1687, 1686, 1685, 1684, 1683, 1682, 1681, 1680, 1679, 1678, 1677, 1676, 1675, 1674, 1673, 1672, 1671, 1670, 1669, 1668, 1667, 1666, 1665, 1664, 1663, 1662, 1661, 1660, 1659, 1658, 1657, 1656, 1655, 1654, 1653, 1652, 1651, 1650, 1649, 1648, 1647, 1646, 1645, 1644, 1643, 1642, 1641, 1640, 1639, 1638, 1637, 1636, 1635, 1634, 1633, 1632, 1631, 1630, 1629, 1628, 1627, 1626, 1625, 1624, 1623, 1622, 1621, 1620, 1619, 1618, 1617, 1616, 1615, 1614, 1613, 1612, 1611, 1610, 1609, 1608, 1607, 1606, 1605, 1604, 1603, 1602, 1601, 1600, 1599, 1598, 1597, 1596, 1595, 1594, 1593, 1592, 1591, 1590, 1589, 1588, 1587, 1586, 1585, 1584, 1583, 1582, 1581, 1580, 1579, 1578, 1577, 1576, 1575, 1574, 1573, 1572, 1571, 1570, 1569, 1568, 1567, 1566, 1565, 1564, 1563, 1562, 1561, 1560, 1559, 1558, 1557, 1556, 1555, 1554, 1553, 1552, 1551, 1550, 1549, 1548, 1547, 1546, 1545, 1544, 1543, 1542, 1541, 1540, 1539, 1538, 1537, 1536, 1535, 1534, 1533, 1532, 1531, 1530, 1529, 1528, 1527, 1526, 1525, 1524, 1523, 1522, 1521, 1520, 1519, 1518, 1517, 1516, 1515, 1514, 1513, 1512, 1511, 1510, 1509, 1508, 1507, 1506, 1505, 1504, 1503, 1502, 1501, 1500, 1499, 1498, 1497, 1496, 1495, 1494, 1493, 1492, 1491, 1490, 1489, 1488, 1487, 1486, 1485, 1484, 1483, 1482, 1481, 1480, 1479, 1478, 1477, 1476, 1475, 1474, 1473, 1472, 1471, 1470, 1469, 1468, 1467, 1466, 1465, 1464, 1463, 1462, 1461, 1460, 1459, 1458, 1457, 1456, 1455, 1454, 1453, 1452, 1451, 1450, 1449, 1448, 1447, 1446, 1445, 1444, 1443, 1442, 1441, 1440, 1439, 1438, 1437, 1436, 1435, 1434, 1433, 1432, 1431, 1430, 1429, 1428, 1427, 1426, 1425, 1424, 1423, 1422, 1421, 1420, 1419, 1418, 1417, 1416, 1415, 1414, 1413, 1412, 1411, 1410, 1409, 1408, 1407, 1406, 1405, 1404, 1403, 1402, 1401, 1400, 1399, 1398, 1397, 1396, 1395, 1394, 1393, 1392, 1391, 1390, 1389, 1388, 1387, 1386, 1385, 1384, 1383, 1382, 1381, 1380, 1379, 1378, 1377, 1376, 1375, 1374, 1373, 1372, 1371, 1370, 1369, 1368, 1367, 1366, 1365, 1364, 1363, 1362, 1361, 1360, 1359, 1358, 1357, 1356, 1355, 1354, 1353, 1352, 1351, 1350, 1349, 1348, 1347, 1346, 1345, 1344, 1343, 1342, 1341, 1340, 1339, 1338, 1337, 1336, 1335, 1334, 1333, 1332, 1331, 1330, 1329, 1328, 1327, 1326, 1325, 1324, 1323, 1322, 1321, 1320, 1319, 1318, 1317, 1316, 1315, 1314, 1313, 1312, 1311, 1310, 1309, 1308, 1307, 1306, 1305, 1304, 1303, 1302, 1301, 1300, 1299, 1298, 1297, 1296, 1295, 1294, 1293, 1292, 1291, 1290, 1289, 1288, 1287, 1286, 1285, 1284, 1283, 1282, 1281, 1280, 1279, 1278, 1277, 1276, 1275
};

//ina219 calibration parameters
const float correcI = 0.84253;
const float correcV = 27.38;


//settings for JV-SOP MPP algorithm search
const int n_average = 60; //30; number of averaged measurements per data point; beta parameter
const int steps = 60;     //5; number of averaged data points constituting a k-step; delta parameter; 10 steps of MPP run ~2 seconds

//Definition of the optimal DAC integer (VGATE)
int countermpp;

//Timing triggers
unsigned long startMillis;
unsigned long currentMillis;

//const unsigned long longperiod = 360000;  //6 min. Portion of time running at MPP condition between MPP search runs.
const unsigned long longperiod = 180000;  //3 min. Portion of time running at MPP condition between MPP search runs.
const unsigned long shortperiod = 3000; // below this time the system is just starting

int loopCount; //global definition of this counter

void setup() {
  Serial.begin(9600);

  //Adafruit MCP4725A dac checkings
  if (! dac.begin(0x60)) {
    Serial.println("Failed to find MCP4725");
    while (1) {
      delay(10);
    }
  }
  dac.begin(0x60);

  //Adafruit INA219 checkings
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219");
    while (1) {
      delay(10);
    }
  }
  ina219.setCalibration_16V_400mA();

  startMillis = millis();  // setting starting time before void loop()
  loopCount = 1;       // setting initial loop id before void loop()
  
}

void loop() {
  currentMillis = millis();

  //search of the MPP voltage
  if  ((currentMillis - startMillis <= shortperiod) || (currentMillis - startMillis >= longperiod)) {
    //if the device has just started up or
    //if the MPP point holding time has passed the long period
    //then launch the MPP search function again.

    // Execute different functions based on the loop count
    if (loopCount >= 1 && loopCount <= 1) {
      mppcalculation(1);
    }
    else if (loopCount >= 2 && loopCount <= 2) {
      mppcalculation(2);
    }
    else if (loopCount >= 3 && loopCount <= 3) {
      mppcalculation(3);
    }
    else if (loopCount >= 4 && loopCount <= 4) {
      mppcalculation(4);
    }
    else if (loopCount >= 5 && loopCount <= 5) {
      mppcalculation(5);
    }
    else if (loopCount >= 6 && loopCount <= 6) {
      mppcalculation(6);
    }
    else if (loopCount >= 7 && loopCount <= 7) {
      mppcalculation(7);
    }
    else if (loopCount >= 8 && loopCount <= 8) {
      mppcalculation(8);
    }

    // Increment loop count
    loopCount++;

    // Reset loop count after reaching 9 loop counts
    if (loopCount >= 9) {
      loopCount = 1;
    }

    startMillis = currentMillis;
  }




  //run SOP stage during longperiod
  SOPmpp(countermpp);

}

// ################FUNCTIONS####################

void SOPmpp(int countermpp) {

  int var = 0;

  while (var <= steps) {
    dac.setVoltage(countermpp, false);
    delay(10);

    float voltageapplied = countermpp * (5000.0 / 4095); //this maps the applied voltage in volts from 0 t 5000 mV
    
    //init variables storing data measured by INA219
    float busvoltage = 0;
    float current_mA = 0;
    float shuntvoltage = 0;

    for (int i = 0; i < n_average; i++) {
      busvoltage = busvoltage + ina219.getBusVoltage_V() * 1000;
      current_mA = current_mA + ina219.getCurrent_mA();
      shuntvoltage = shuntvoltage + ina219.getShuntVoltage_mV();
      delay(1);
    }

    float busvoltagesc = busvoltage / n_average;
    float current_mAsc = ( correcI * current_mA ) / n_average;
    float shuntvoltagesc = shuntvoltage / n_average;
    float voltagesc = busvoltagesc + shuntvoltagesc + correcV;
    float power = voltagesc * current_mAsc / 1000.0; // mW
    delay(10);



    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
    Serial.print("MPP"); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.println(temperature);

    var++;
  }


}

int mppcalculation(int listNumber) {

  float powermpp = -1.0;// initial value to ensure that it picks up any positive value
  countermpp = 0;// initial value to ensure that it picks up any positive value

  const int* dacLookupList;
  int dacSteps;

  if (listNumber == 1)
  {
    dacLookupList = DACLookup_list1;
    dacSteps = dac_steps1;
    //serial.println("using list1");
  }
  if (listNumber == 2)
  {
    dacLookupList = DACLookup_list2;
    dacSteps = dac_steps2;
    //serial.println("using list2");
  }
  if (listNumber == 3)
  {
    dacLookupList = DACLookup_list3;
    dacSteps = dac_steps3;
    //serial.println("using list3");
  }
  if (listNumber == 4)
  {
    dacLookupList = DACLookup_list4;
    dacSteps = dac_steps4;
    //serial.println("using list4");
  }
  if (listNumber == 5)
  {
    dacLookupList = DACLookup_list5;
    dacSteps = dac_steps5;
    //serial.println("using list5");
  }
  if (listNumber == 6)
  {
    dacLookupList = DACLookup_list6;
    dacSteps = dac_steps6;
    //serial.println("using list6");
  }
  if (listNumber == 7)
  {
    dacLookupList = DACLookup_list7;
    dacSteps = dac_steps7;
    //serial.println("using list7");
  }
  if (listNumber == 8)
  {
    dacLookupList = DACLookup_list8;
    dacSteps = dac_steps8;
    //serial.println("using list8");
  }


  for (int j = 0; j < dacSteps  ; j = j + 1) {

    int counter = pgm_read_word(&(dacLookupList[j]));
    dac.setVoltage(counter, false);// the dac sets the voltage on the mosfet
    delay(10);


    float voltageapplied = counter * (5000.0 / 4095); //this maps the applied voltage in volts from 0 t 5000 mV

    //init variables storing data measured by INA219
    float busvoltage = 0;
    float current_mA = 0;
    float shuntvoltage = 0;


    //n_average: number of measurements to be averaged
    for (int i = 0; i < n_average; i++) {
      busvoltage = busvoltage + ina219.getBusVoltage_V() * 1000;
      current_mA = current_mA + ina219.getCurrent_mA();
      shuntvoltage = shuntvoltage + ina219.getShuntVoltage_mV();

      delay(1);
    }
    float busvoltagesc = busvoltage / n_average;
    float current_mAsc = ( correcI * current_mA ) / n_average;
    float shuntvoltagesc = shuntvoltage / n_average;
    float voltagesc = busvoltagesc + shuntvoltagesc + correcV;
    float power = voltagesc * current_mAsc / 1000.0; // mW



    //Classic MPP Search. Fetch dac step recording the MPP
    if ((power >= powermpp)) {
      powermpp = power;
      countermpp = counter;
    }

    //solar cell temperature measurement
    float temperature = termistorT();


    //serial output
    Serial.print("JV"); Serial.print("\t");
    Serial.print(counter); Serial.print("\t");
    Serial.print(voltageapplied); Serial.print("\t");
    Serial.print(voltagesc); Serial.print("\t");
    Serial.print(current_mAsc); Serial.print("\t");
    Serial.print(power); Serial.print("\t");
    Serial.print(countermpp); Serial.print("\t");
    Serial.print(powermpp); Serial.print("\t");
    Serial.println(temperature);
  }
  return countermpp;
}


float termistorT() {
  float raw = 0;
  for (int i = 0; i < 30; i++) {
    raw = raw + analogRead(SensorPIN);
    delay(1);
  }
  raw = raw / 30.0;
  float V =  raw / 1024 * Vcc;
  float R = (Rc * V ) / (Vcc - V);
  float logRR  = log(R);
  float kelvin = 1.0 / (A + B * logRR + C * logRR * logRR * logRR );
  float celsius = kelvin - 273.15;
  return celsius;
}
