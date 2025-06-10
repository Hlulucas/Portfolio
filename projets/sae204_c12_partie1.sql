-- LEMARIE-BELLIOT Nolann Hurtrel Lucas
-- Suppression sécurisée des tables dans l'ordre inverse de dépendance
DROP TABLE IF EXISTS partie1._resultat CASCADE;
DROP TABLE IF EXISTS partie1._programme CASCADE;
DROP TABLE IF EXISTS partie1._module CASCADE;
DROP TABLE IF EXISTS partie1._inscription CASCADE;
DROP TABLE IF EXISTS partie1._groupe_tp CASCADE;
DROP TABLE IF EXISTS partie1._etudiant CASCADE;
DROP TABLE IF EXISTS partie1._candidat CASCADE;
DROP TABLE IF EXISTS partie1._individu CASCADE;
DROP TABLE IF EXISTS partie1._semestre CASCADE;

-- Suppression du schéma si vide
DROP SCHEMA IF EXISTS partie1 CASCADE;

-- Re-création du schéma
CREATE SCHEMA partie1;

-- Table Individu
CREATE TABLE partie1._individu (
    id_individu INT PRIMARY KEY,
    nom VARCHAR(100) NOT NULL,
    prenom VARCHAR(100) NOT NULL,
    date_naissance DATE NOT NULL,
    code_postal VARCHAR(10),
    ville VARCHAR(100),
    sexe CHAR(1),
    nationalite VARCHAR(50),
    ine VARCHAR(20)
);

-- Table Candidat
CREATE TABLE partie1._candidat (
    id_candidat INT PRIMARY KEY,
    id_individu INT NOT NULL,
    classement VARCHAR(10),
    boursier_lycee BOOLEAN,
    profil_candidat VARCHAR(100),
    etablissement VARCHAR(100),
    dept_etablissement VARCHAR(3),
    ville_etablissement VARCHAR(100),
    niveau_etude VARCHAR(50),
    type_formation_prec VARCHAR(50),
    serie_prec VARCHAR(10),
    dominante_prec VARCHAR(50),
    specialite_prec VARCHAR(50),
    lv1 VARCHAR(30),
    lv2 VARCHAR(30),
    CONSTRAINT fk_candidat_individu FOREIGN KEY (id_individu)
        REFERENCES partie1._individu (id_individu)
);

-- Table Etudiant
CREATE TABLE partie1._etudiant (
    id_etudiant INT PRIMARY KEY,
    id_candidat INT NOT NULL,
    cat_socio_etu VARCHAR(50),
    cat_socio_parent VARCHAR(50),
    bourse_superieur BOOLEAN,
    mention_bac VARCHAR(50),
    serie_bac VARCHAR(10),
    dominante_bac VARCHAR(50),
    specialite_bac VARCHAR(50),
    mois_annee_obtention_bac CHAR(7),
    CONSTRAINT fk_etudiant_candidat FOREIGN KEY (id_candidat)
        REFERENCES partie1._candidat (id_candidat)
);

-- Table Semestre
CREATE TABLE partie1._semestre (
    id_semestre VARCHAR(10) PRIMARY KEY,
    annee INT NOT NULL,
    parcours VARCHAR(10)
);

-- Table Groupe TP
CREATE TABLE partie1._groupe_tp (
    id_groupe_tp INT PRIMARY KEY,
    nom_groupe VARCHAR(20) UNIQUE NOT NULL
);

-- Table Inscription
CREATE TABLE partie1._inscription (
    id_etudiant INT,
    id_semestre VARCHAR(10),
    id_groupe_tp INT,
    amenagement_evaluation VARCHAR(255),
    PRIMARY KEY (id_etudiant, id_semestre),
    CONSTRAINT fk_inscription_etudiant FOREIGN KEY (id_etudiant)
        REFERENCES partie1._etudiant (id_etudiant),
    CONSTRAINT fk_inscription_semestre FOREIGN KEY (id_semestre)
        REFERENCES partie1._semestre (id_semestre),
    CONSTRAINT fk_inscription_groupe FOREIGN KEY (id_groupe_tp)
        REFERENCES partie1._groupe_tp (id_groupe_tp)
);

-- Table Module
CREATE TABLE partie1._module (
    id_module VARCHAR(10) PRIMARY KEY,
    libelle VARCHAR(100) NOT NULL,
    ue VARCHAR(10) NOT NULL
);

-- Table Programme (association Module <-> Semestre avec coefficient)
CREATE TABLE partie1._programme (
    id_module VARCHAR(10),
    id_semestre VARCHAR(10),
    coefficient NUMERIC(3,1),
    PRIMARY KEY (id_module, id_semestre),
    CONSTRAINT fk_programme_module FOREIGN KEY (id_module)
        REFERENCES partie1._module (id_module),
    CONSTRAINT fk_programme_semestre FOREIGN KEY (id_semestre)
        REFERENCES partie1._semestre (id_semestre)
);

-- Table Resultat
CREATE TABLE partie1._resultat (
    id_etudiant INT,
    id_module VARCHAR(10),
    id_semestre VARCHAR(10),
    moyenne NUMERIC(4,2) CHECK (moyenne BETWEEN 0 AND 20),
    PRIMARY KEY (id_etudiant, id_module, id_semestre),
    CONSTRAINT fk_resultat_etudiant FOREIGN KEY (id_etudiant)
        REFERENCES partie1._etudiant (id_etudiant),
    CONSTRAINT fk_resultat_module FOREIGN KEY (id_module)
        REFERENCES partie1._module (id_module),
    CONSTRAINT fk_resultat_semestre FOREIGN KEY (id_semestre)
        REFERENCES partie1._semestre (id_semestre)
);
