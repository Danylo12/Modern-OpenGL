#version 330 core

layout(location=0) in
vec3 polozenie_in;

layout(location=3) in
vec4 kolor_in;

uniform bool UwzglednijKolorWerteksu = true;

uniform vec4 Kolor = vec4(1.0, 1.0, 0.0, 1.0);

const mat4 macierzJednostkowa = mat4(1.0);

uniform mat4 macierzSwiata = macierzJednostkowa;

uniform mat4 macierzWidoku = macierzJednostkowa;

uniform mat4 macierzRzutowania = macierzJednostkowa;

mat4 macierzMVP = macierzRzutowania * macierzWidoku * macierzSwiata;

out
vec4 polozenie;

out
vec4 kolor;

void main(void)
{

    polozenie = vec4(polozenie_in, 1.0);

    gl_Position = macierzMVP * polozenie;

    polozenie = macierzSwiata * polozenie;

    kolor = (UwzglednijKolorWerteksu) ? kolor_in : Kolor;

}