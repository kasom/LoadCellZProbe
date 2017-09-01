aluminium_height=20;
loadcell_size=12.5;
mount_depth=20;
mount_long=25;
mount_loc=15/2;
safety=0.3;
screw_head_thick=3;
screw_head_dia=8;

rotate([0,270,0])
difference() {
    linear_extrude(aluminium_height) square([mount_depth,mount_long],true);

    translate([(mount_depth-loadcell_size)/2,0,(aluminium_height-loadcell_size)/2-safety/2])
        linear_extrude(loadcell_size+safety)
            square([loadcell_size+safety,mount_depth*2+safety],true);
    
    translate([(mount_depth-loadcell_size)/2,mount_loc,-aluminium_height/2])
        linear_extrude(aluminium_height*2+safety) circle(d=5.2,$fn=36);

    translate([(mount_depth-loadcell_size)/2,-mount_loc,-aluminium_height/2])
        linear_extrude(aluminium_height*2) circle(d=5.2,$fn=36);


    translate([-mount_depth,-mount_loc,aluminium_height/2])
        rotate([0,90,0])
            linear_extrude(mount_depth*2)
                circle(d=4.2,$fn=36);

    translate([-mount_depth,mount_loc,aluminium_height/2])
        rotate([0,90,0])
            linear_extrude(mount_depth*2)
                circle(d=4.2,$fn=36);

    translate([(mount_depth-loadcell_size)-mount_depth/2-screw_head_thick,mount_loc,aluminium_height/2])
        rotate([0,90,0])
            linear_extrude(screw_head_thick)
                circle(d=screw_head_dia,$fn=36);

    translate([(mount_depth-loadcell_size)-mount_depth/2-screw_head_thick,-mount_loc,aluminium_height/2])
        rotate([0,90,0])
            linear_extrude(screw_head_thick)
                circle(d=screw_head_dia,$fn=36);
}

