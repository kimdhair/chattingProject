-- MySQL dump 10.13  Distrib 8.0.34, for Win64 (x86_64)
--
-- Host: localhost    Database: chattingproject
-- ------------------------------------------------------
-- Server version	8.1.0

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `chatting_log`
--

DROP TABLE IF EXISTS `chatting_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `chatting_log` (
  `INDEX` int NOT NULL AUTO_INCREMENT,
  `ROOM` varchar(45) DEFAULT 'FreeRoom',
  `ID` varchar(45) NOT NULL,
  `DM` varchar(45) DEFAULT '0',
  `SAY` varchar(100) DEFAULT '0',
  `DATETIME` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`INDEX`),
  KEY `id_idx` (`ID`),
  CONSTRAINT `id` FOREIGN KEY (`ID`) REFERENCES `members` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=722 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chatting_log`
--

LOCK TABLES `chatting_log` WRITE;
/*!40000 ALTER TABLE `chatting_log` DISABLE KEYS */;
INSERT INTO `chatting_log` VALUES (698,'room1','aa','0','','2023-10-24 10:13:27'),(699,'room1','aa','0','hello, i made room1 ^^','2023-10-24 10:14:24'),(700,'room1','aa','0','nice to meet you','2023-10-24 10:14:36'),(702,'FreeRoom','aa','0','','2023-10-24 10:15:57'),(703,'FreeRoom','aa','0','i am king of the World!!!','2023-10-24 10:16:09'),(713,'room1','aa','0','','2023-10-24 10:24:23'),(714,'room1','aa','0','hi','2023-10-24 10:24:27'),(715,'room1','bb','0','','2023-10-24 10:24:35'),(716,'room1','bb','0','hi','2023-10-24 10:24:38'),(717,'room1','aa','0','i quit','2023-10-24 10:24:46'),(718,'FreeRoom','aa','0','','2023-10-24 10:25:03'),(719,'FreeRoom','aa','0','fire!!!','2023-10-24 10:25:06'),(720,'FreeRoom','aa','bb','','2023-10-24 10:25:20'),(721,'FreeRoom','aa','bb','i will kill you','2023-10-24 10:25:26');
/*!40000 ALTER TABLE `chatting_log` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-10-24 13:10:33
