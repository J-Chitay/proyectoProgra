create table libros (
	titulo varchar(50) not null,
	autor varchar(50) not null,
	isbn int primary key,
	editorial varchar(50) not null,
	aniopublicacion int not null,
	areacientifica varchar(50) not null,
	disponibilidad int not null
);

drop table libros;

select * from libros;

CREATE TABLE usuarios (
    id SERIAL PRIMARY KEY,
    usuario VARCHAR(50) NOT NULL,
    contrasena VARCHAR(100) NOT NULL,
    tipo VARCHAR(20) NOT NULL  -- Puede ser 'admin' o 'normal'
);

select * from usuarios;

INSERT INTO usuarios (usuario, contrasena, tipo) VALUES
('admin', 'admin123', 'admin'),
('juan', 'juan123', 'normal');

CREATE TABLE prestamos (
    id_prestamo SERIAL PRIMARY KEY,
    isbn INT NOT NULL,
    titulo VARCHAR(50) NOT NULL,
    id_usuario INT NOT NULL,
    fecha_prestamo DATE NOT NULL,
    FOREIGN KEY (isbn) REFERENCES libros(isbn),
    FOREIGN KEY (id_usuario) REFERENCES usuarios(id)
);

select * from prestamos;

drop table prestamos;