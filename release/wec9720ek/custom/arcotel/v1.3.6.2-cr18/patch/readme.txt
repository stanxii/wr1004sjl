1. chnage logo

2. color.css
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
.footerbody {
    border-top: #333333 1px solid;
    border-bottom: #333333 1px solid;
    background: #333333 none;
}
.footertd {
    background-color: #333333;
}
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

3. stylemain.css
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
td.maintitle {
    background-color: #333333;
    font-weight: bold;
    font-size: x-small;
    font-family: "tahoma", "sans-serif", "arial", "helvetica";
    height:25px;
    color:white;
    border-top-left-radius: 5px;
    border-top-right-radius: 5px;
    padding: .2em 1em .1em 1em;
}
td.mainline {
    background-color: #333333;
    height:2px;
}
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

4. wecNewCnu.html
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
<select id='devModel' size=1>
	<option value=0>Auto Scane</option>
	<option value=1>EOC-S100-4F</option>
</select>
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

5. public.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define CUSTOM_LOGO_ID CUSTOM_LOGO_ALCOTEL
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++