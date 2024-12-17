$fn = 50;

thick = 1;
w = 50;
h = 80;
r = 5;

rim_h = 29;
pcb_elevation = 8;
pcb_thick = 1.8;

head_d = 8;
body_d = 4;

// y coordinate of upper screwholes
yy_up = h / 2 - 5 - 3 - 8 - 5;
yy_down = -h / 2 + 3;
xx_left = -w / 2 + 3;
xx_right = w / 2 - 3;

module base_shape(w, h, r)
{
    minkowski()
    {
        square([w,h], center = true);
        circle(r);
    }
}

module base_2d()
{
    difference()
    {
        base_shape(w, h, r);
        translate([0, h/2 - 5, 0]) union()
        {
            hull()
            {
                circle(d = body_d);
                translate([0, -body_d*1.5, 0]) circle(d = body_d);
            }
            translate([0, -body_d*1.5, 0]) circle(d = head_d);
        }
    }
}

module rim_2d()
{
    difference()
    {
        base_shape(w, h, r);
        base_shape(w, h, r - thick);
    }
}

module arch(size)
{
    rotate([90, 0, 90]) linear_extrude(thick + 1, center = true) difference()
    {
        square(size);
        intersection()
        {
            circle(r = size);
            square(size + 1);
        }
    }
}

module rimhole(size, r)
{
    zthick = rim_h - thick - pcb_elevation - pcb_thick;
    translate([0, 0, zthick/2 + thick + pcb_elevation + pcb_thick]) cube([thick + 1, size, zthick], center = true);
    translate([0, -r - size/2, rim_h - r]) arch(r);
    translate([0, r + size/2, rim_h - r]) rotate([0, 0, 180]) arch(r);
}

module arkhole(r)
{
    translate([-thick/2, -10/2 + r, r]) minkowski()
    {
        cube([thick, 10 - 2*r, 10 - 2*r]);
        sphere(r = r);
    }
}

module usbhole(r)
{
    //translate([0, 0, 3.5]) cube([12, thick + 1, 7], true);
    translate([-12/2 + r, -thick/2, r]) minkowski()
    {
        cube([12 - 2*r, thick, 7 - 2*r]);
        sphere(r = r);
    }
}

module rim()
{
    difference()
    {
        linear_extrude(rim_h) rim_2d();
        translate([-w/2 - r + thick/2, yy_up - 10, thick + pcb_elevation + pcb_thick])
            arkhole(2);
         translate([w/2 + r - thick/2, yy_up - 10, thick + pcb_elevation + pcb_thick])
            arkhole(2);
         translate([0, -h/2 - r + thick/2, thick + pcb_elevation + pcb_thick - 3.5 + 0.9])
            usbhole(2);
        /*
        translate([-w/2 - r + thick/2, yy_up - 10, 0])
            rimhole(9, 2);
         translate([w/2 + r - thick/2, yy_up - 10, 0])
            rimhole(9, 2);
         translate([0, -h/2 - r + thick/2, 0])
         rotate([0, 0, 90])
            rimhole(15, 2);
        */
    }
}

module screwstand()
{
    cylinder(pcb_elevation, d1 = 9, d2 = 7);
}

module screwhole()
{
    //cylinder(2, d = 5);
    translate([0, 0, thick]) cylinder(pcb_elevation + 1, d = 2.2);
}

module base()
{
    difference()
    {
        union()
        {
            linear_extrude(thick) base_2d();
            translate([xx_left, yy_down, thick]) screwstand();
            translate([xx_right, yy_down, thick]) screwstand();
            translate([xx_left, yy_up, thick]) screwstand();
            translate([xx_right, yy_up, thick]) screwstand();
        }
        translate([xx_left, yy_down, 0]) screwhole();
        translate([xx_right, yy_down, 0]) screwhole();
        translate([xx_left, yy_up, 0]) screwhole();
        translate([xx_right, yy_up, 0]) screwhole();
    }
}

lid_screwstand_h = rim_h - thick*2 - pcb_elevation - pcb_thick;

echo("Base screwstand height: ", pcb_elevation);
echo("Lid screwstand height: ", lid_screwstand_h);

module lid_screwstand()
{
    cylinder(lid_screwstand_h, d1 = 9, d2 = 7);
}

module lid_screwhole()
{
    translate([0, 0, -thick]) cylinder(lid_screwstand_h + thick - 1, d = 6);
    cylinder(lid_screwstand_h + 0.5, d = 1.9);
}

module lid_ark_hole(width)
{
    square([width, 10], true);
}

module lid()
{
    tolerance = 0.1;
    difference()
    {
        union()
        {
            linear_extrude(thick) difference()
            {
                rr = r - thick - tolerance;
                base_shape(w, h, rr);
                ww = 9;
                translate([-w/2-rr +ww/2, yy_up - 10]) lid_ark_hole(ww);
                translate([w/2+rr -ww/2, yy_up - 10]) lid_ark_hole(ww);
            }
            translate([xx_left, yy_down, thick]) lid_screwstand();
            translate([xx_right, yy_down, thick]) lid_screwstand();
            translate([xx_left, yy_up, thick]) lid_screwstand();
            translate([xx_right, yy_up, thick]) lid_screwstand();
        }
        translate([xx_left, yy_down, thick]) lid_screwhole();
        translate([xx_right, yy_down, thick]) lid_screwhole();
        translate([xx_left, yy_up, thick]) lid_screwhole();
        translate([xx_right, yy_up, thick]) lid_screwhole();
    }
}

module holder()
{
    difference()
    {
        translate([0, 0, rim_h]) linear_extrude(80 - rim_h) rim_2d();
        translate([-w/2 - r - 1, 0, -0.5]) cube([w + 2*r + 2, h, 81]);
        translate([0, 20, 60]) sphere(d = 120);
    }
}

//lid();
base();
rim();
//holder();
//translate([0, 0, rim_h]) rotate([0, 180, 0]) lid();


echo("Horizontal screw distance: ", xx_right - xx_left);
echo("Vertical screw distance: ", yy_up - yy_down);