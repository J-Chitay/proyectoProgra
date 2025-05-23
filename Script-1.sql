create table libros (
	titulo varchar(50) not null,
	autor varchar(50) not null,
	isbn int primary key,
	editorial varchar(50) not null,
	aniopublicacion int not null,
	areacientifica varchar(50) not null
	
ALTER TABLE libros ADD COLUMN cantidad_disponible INT NOT NULL DEFAULT 0;

	
);
CREATE TABLE usuarios (
    id SERIAL PRIMARY KEY,
    usuario VARCHAR(50) NOT NULL,
    contrasena VARCHAR(100) NOT NULL,
    tipo VARCHAR(20) NOT NULL  -- Puede ser 'admin' o 'normal'
);

select * from prestamos;
INSERT INTO usuarios (usuario, contrasena, tipo) VALUES

('b', 'b', 'normal');


update libros 
set ccontidad_disponible = disponibilidad
where tipo = 'normal';

CREATE TABLE prestamos (
    id SERIAL PRIMARY KEY,
    isbn INT REFERENCES libros(isbn),
    fecha_prestamo TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
alter table prestamos add column usuario varchar(50);

select * from prestamos;
select * from usuarios;
select * from libros;

DELETE FROM libros;
ALTER TABLE libros RENAME COLUMN cantidad_disponible TO disponibilidad;

