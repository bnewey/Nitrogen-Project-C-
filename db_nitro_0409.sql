-- MySQL dump 10.13  Distrib 5.7.29, for Linux (x86_64)
--
-- Host: localhost    Database: db_nitro
-- ------------------------------------------------------
-- Server version	5.7.29-0ubuntu0.18.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `air_compressor`
--

DROP TABLE IF EXISTS `air_compressor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `air_compressor` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `air_compressor_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `air_compressor`
--

LOCK TABLES `air_compressor` WRITE;
/*!40000 ALTER TABLE `air_compressor` DISABLE KEYS */;
INSERT INTO `air_compressor` VALUES (1,104,106,'2020-03-31 12:23:21',1),(2,56,58,'2020-03-31 12:28:41',1),(3,56,58,'2020-03-31 12:34:02',1),(4,56,58,'2020-03-31 12:39:22',1),(5,0,3,'2020-03-31 16:10:12',1),(6,0,3,'2020-04-01 11:43:55',1),(7,2,0,'2020-04-02 08:54:26',1),(8,2,0,'2020-04-02 09:22:31',1),(9,2,0,'2020-04-02 09:32:46',1),(10,2,0,'2020-04-02 09:46:11',1);
/*!40000 ALTER TABLE `air_compressor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `air_dryer`
--

DROP TABLE IF EXISTS `air_dryer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `air_dryer` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `air_dryer_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `air_dryer`
--

LOCK TABLES `air_dryer` WRITE;
/*!40000 ALTER TABLE `air_dryer` DISABLE KEYS */;
INSERT INTO `air_dryer` VALUES (1,108,110,'2020-03-31 12:23:21',2),(2,60,62,'2020-03-31 12:28:41',2),(3,60,62,'2020-03-31 12:34:02',2),(4,60,62,'2020-03-31 12:39:22',2),(5,2,0,'2020-03-31 16:10:12',2),(6,2,0,'2020-04-01 11:43:55',2),(7,105,1,'2020-04-02 08:54:26',2),(8,55,1,'2020-04-02 09:22:31',2),(9,55,1,'2020-04-02 09:32:46',2),(10,55,1,'2020-04-02 09:46:11',2);
/*!40000 ALTER TABLE `air_dryer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `generator`
--

DROP TABLE IF EXISTS `generator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `generator` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `generator_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `generator`
--

LOCK TABLES `generator` WRITE;
/*!40000 ALTER TABLE `generator` DISABLE KEYS */;
INSERT INTO `generator` VALUES (1,165,165,'2020-03-31 12:23:21',7),(2,165,165,'2020-03-31 12:28:41',7),(3,165,165,'2020-03-31 12:34:02',7),(4,165,165,'2020-03-31 12:39:22',7),(5,48,48,'2020-03-31 16:10:12',7),(6,0,0,'2020-04-01 11:43:55',7),(7,48,48,'2020-04-02 08:54:26',7),(8,48,48,'2020-04-02 09:22:31',7),(9,48,48,'2020-04-02 09:32:46',7),(10,48,48,'2020-04-02 09:46:11',7);
/*!40000 ALTER TABLE `generator` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `machines`
--

DROP TABLE IF EXISTS `machines`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `machines` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(30) NOT NULL,
  `machine_table_name` varchar(25) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_table_name_idx` (`machine_table_name`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `machines`
--

LOCK TABLES `machines` WRITE;
/*!40000 ALTER TABLE `machines` DISABLE KEYS */;
INSERT INTO `machines` VALUES (1,'Air Compressor','air_compressor'),(2,'Air Dryer','air_dryer'),(3,'Tank 1','tank_1'),(4,'Tank 1_3','tank_1_3'),(5,'Tank 2_3','tank_2_3'),(6,'Tank 3_3','tank_3_3'),(7,'Generator','generator'),(8,'Nitrogen Tank','nitrogen_tank');
/*!40000 ALTER TABLE `machines` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mode_variables`
--

DROP TABLE IF EXISTS `mode_variables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mode_variables` (
  `value` int(4) NOT NULL,
  `tag` varchar(30) NOT NULL,
  `name` varchar(30) NOT NULL,
  `id` int(11) NOT NULL AUTO_INCREMENT,
  UNIQUE KEY `tag` (`tag`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=29 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mode_variables`
--

LOCK TABLES `mode_variables` WRITE;
/*!40000 ALTER TABLE `mode_variables` DISABLE KEYS */;
INSERT INTO `mode_variables` VALUES (350,'high_pressure_thres','High Pressure Threshold',24),(86,'low_pressure_thres','Low Pressure Threshold',26),(400,'max_high_pressure','Max High Pressure',23),(94,'max_low_pressure','Max Low Pressure',25),(60,'min_low_pressure','Minimum Low Pressure',27),(2,'shut_down_counter','ShutDown Counter',28),(2,'timer_bleed_relay_m1','Bleed Relay Mode 1',22),(5,'timer_bleed_relay_m45','Bleed Relay Mode 4&5',21),(121,'timer_mode2_wait','Mode 2 Wait Timer',15),(10,'timer_mode4_wait','Mode 4 Wait Timer',16),(30,'timer_motor_relay','Motor Relay Timer',17),(30,'timer_shut_down_counter','ShutDown Counter Timer',20),(2,'timer_start_relay','Start Relay Timer',18),(2,'timer_stop_relay','Stop Relay Timer',19);
/*!40000 ALTER TABLE `mode_variables` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `nitrogen_tank`
--

DROP TABLE IF EXISTS `nitrogen_tank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `nitrogen_tank` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `nitrogen_tank_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `nitrogen_tank`
--

LOCK TABLES `nitrogen_tank` WRITE;
/*!40000 ALTER TABLE `nitrogen_tank` DISABLE KEYS */;
INSERT INTO `nitrogen_tank` VALUES (1,165,165,'2020-03-31 12:23:21',8),(2,165,165,'2020-03-31 12:28:41',8),(3,165,165,'2020-03-31 12:34:02',8),(4,165,165,'2020-03-31 12:39:22',8),(5,48,48,'2020-03-31 16:10:12',8),(6,0,0,'2020-04-01 11:43:55',8),(7,48,48,'2020-04-02 08:54:26',8),(8,48,48,'2020-04-02 09:22:31',8),(9,48,48,'2020-04-02 09:32:46',8),(10,48,48,'2020-04-02 09:46:11',8);
/*!40000 ALTER TABLE `nitrogen_tank` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tank_1`
--

DROP TABLE IF EXISTS `tank_1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tank_1` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `tank_1_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tank_1`
--

LOCK TABLES `tank_1` WRITE;
/*!40000 ALTER TABLE `tank_1` DISABLE KEYS */;
INSERT INTO `tank_1` VALUES (1,112,114,'2020-03-31 12:23:21',3),(2,64,66,'2020-03-31 12:28:41',3),(3,64,66,'2020-03-31 12:34:02',3),(4,64,66,'2020-03-31 12:39:22',3),(5,68,1,'2020-03-31 16:10:12',3),(6,68,1,'2020-04-01 11:43:55',3),(7,54,0,'2020-04-02 08:54:26',3),(8,54,0,'2020-04-02 09:22:31',3),(9,54,0,'2020-04-02 09:32:46',3),(10,54,0,'2020-04-02 09:46:11',3);
/*!40000 ALTER TABLE `tank_1` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tank_1_3`
--

DROP TABLE IF EXISTS `tank_1_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tank_1_3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `tank_1_3_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tank_1_3`
--

LOCK TABLES `tank_1_3` WRITE;
/*!40000 ALTER TABLE `tank_1_3` DISABLE KEYS */;
INSERT INTO `tank_1_3` VALUES (1,116,118,'2020-03-31 12:23:21',4),(2,68,70,'2020-03-31 12:28:41',4),(3,68,70,'2020-03-31 12:34:02',4),(4,68,70,'2020-03-31 12:39:22',4),(5,129,0,'2020-03-31 16:10:12',4),(6,129,0,'2020-04-01 11:43:55',4),(7,0,0,'2020-04-02 08:54:26',4),(8,0,0,'2020-04-02 09:22:31',4),(9,0,0,'2020-04-02 09:32:46',4),(10,0,0,'2020-04-02 09:46:11',4);
/*!40000 ALTER TABLE `tank_1_3` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tank_2_3`
--

DROP TABLE IF EXISTS `tank_2_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tank_2_3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `tank_2_3_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tank_2_3`
--

LOCK TABLES `tank_2_3` WRITE;
/*!40000 ALTER TABLE `tank_2_3` DISABLE KEYS */;
INSERT INTO `tank_2_3` VALUES (1,120,122,'2020-03-31 12:23:21',5),(2,72,74,'2020-03-31 12:28:41',5),(3,72,74,'2020-03-31 12:34:02',5),(4,72,74,'2020-03-31 12:39:22',5),(5,0,0,'2020-03-31 16:10:12',5),(6,0,0,'2020-04-01 11:43:55',5),(7,0,0,'2020-04-02 08:54:26',5),(8,0,0,'2020-04-02 09:22:31',5),(9,0,0,'2020-04-02 09:32:46',5),(10,0,0,'2020-04-02 09:46:11',5);
/*!40000 ALTER TABLE `tank_2_3` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tank_3_3`
--

DROP TABLE IF EXISTS `tank_3_3`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tank_3_3` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `temp` int(11) NOT NULL,
  `pressure` int(11) NOT NULL,
  `read_date` datetime NOT NULL,
  `machine_id` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `machine_id` (`machine_id`),
  CONSTRAINT `tank_3_3_ibfk_1` FOREIGN KEY (`machine_id`) REFERENCES `machines` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tank_3_3`
--

LOCK TABLES `tank_3_3` WRITE;
/*!40000 ALTER TABLE `tank_3_3` DISABLE KEYS */;
INSERT INTO `tank_3_3` VALUES (1,165,165,'2020-03-31 12:23:21',6),(2,165,165,'2020-03-31 12:28:41',6),(3,165,165,'2020-03-31 12:34:02',6),(4,165,165,'2020-03-31 12:39:22',6),(5,0,0,'2020-03-31 16:10:12',6),(6,0,0,'2020-04-01 11:43:55',6),(7,0,3,'2020-04-02 08:54:26',6),(8,0,3,'2020-04-02 09:22:31',6),(9,0,3,'2020-04-02 09:32:46',6),(10,0,3,'2020-04-02 09:46:11',6);
/*!40000 ALTER TABLE `tank_3_3` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2020-04-09  9:27:35
