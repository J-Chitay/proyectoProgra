PGDMP         ,                }            postgres    15.12    15.12 	    �           0    0    ENCODING    ENCODING        SET client_encoding = 'UTF8';
                      false            �           0    0 
   STDSTRINGS 
   STDSTRINGS     (   SET standard_conforming_strings = 'on';
                      false            �           0    0 
   SEARCHPATH 
   SEARCHPATH     8   SELECT pg_catalog.set_config('search_path', '', false);
                      false            �           1262    5    postgres    DATABASE     n   CREATE DATABASE postgres WITH TEMPLATE = template0 ENCODING = 'UTF8' LOCALE_PROVIDER = libc LOCALE = 'es-MX';
    DROP DATABASE postgres;
                postgres    false            �           0    0    DATABASE postgres    COMMENT     N   COMMENT ON DATABASE postgres IS 'default administrative connection database';
                   postgres    false    3321                        2615    2200    public    SCHEMA        CREATE SCHEMA public;
    DROP SCHEMA public;
                pg_database_owner    false            �           0    0    SCHEMA public    COMMENT     6   COMMENT ON SCHEMA public IS 'standard public schema';
                   pg_database_owner    false    5            �            1259    16398    libros    TABLE       CREATE TABLE public.libros (
    titulo character varying NOT NULL,
    autor character varying NOT NULL,
    isbn character varying NOT NULL,
    editorial character varying NOT NULL,
    aniopublicacion character varying NOT NULL,
    areacientifica character varying NOT NULL
);
    DROP TABLE public.libros;
       public         heap    postgres    false    5            �          0    16398    libros 
   TABLE DATA           a   COPY public.libros (titulo, autor, isbn, editorial, aniopublicacion, areacientifica) FROM stdin;
    public          postgres    false    215          �   5   x�K�J�-��7��M,���4���LM�,�/�L�1�420�L,JM4����� Q}     