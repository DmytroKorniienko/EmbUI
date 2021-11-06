
var global = {menu_id:0, menu: [], value:{}};

var render = function(){
	var tmpl_menu = new mustache(go("#tmpl_menu")[0],{
		on_page: function(d,id) {
			out.menu_change(id);
		}
	}),
	fn_section = {
		on_input: function(d, id){
			var value = this.value, type = this.type;
			if (type == "range") go("#"+id+"-val").html(": " + value);
			if (type == "text" || type == "password") go("#"+id+"-val").html(" ("+value.length+")");
			if (type == "color") go("#"+id+"-val").html(" ("+value+")");
			if (this.id != id){
				custom_hook(this.id, d, id);
			}
		},
		on_change: function(d, id) {
			var value = this.value, type = this.type;
			if (type == "checkbox"){
				var chbox=document.getElementById(id);
				if (chbox.checked) value = "1";		// send 'checked' state as "1"
				else value = "0";
			}
			if (this.id != id){
				custom_hook(this.id, d, id);
			}
			var data = {}; data[id] = (value !== undefined)? value : null;
			if (this.className.includes("confirm")){
				if (confirm(this.nextElementSibling.textContent))
					ws.send_post(data);
				}
				else
					ws.send_post(data);
		},
		on_showhide: function(d, id) {
			go("#"+id).showhide();
		},
		on_submit: function(d, id, val) {
			var form = go("#"+id), data = go.formdata(go("input, textarea, select", form));
			data[id] = val || null;
			if (this.className.includes("confirm")){
				if (confirm(this.nextElementSibling.textContent))
					ws.send_post(data);
				}
				else
					ws.send_post(data);
		}
	},
	tmpl_section = new mustache(go("#tmpl_section")[0], fn_section),
	tmpl_section_main = new mustache(go("#tmpl_section_main")[0], fn_section),
	tmpl_content = new mustache(go("#tmpl_content")[0], fn_section),
	out = {
		lockhist: false,
		history: function(hist){
			history.pushState({hist:hist}, '', '?'+hist);
		},
		menu_change: function(menu_id){
			global.menu_id = menu_id;
			this.menu();
			go("#main > div").display("none");
			// go("#main #"+menu_id).display("block");
			var data = {}; data[menu_id] = null
			ws.send_post(data);
		},
		menu: function(){
			go("#menu").clear().append(tmpl_menu.parse(global));
		},
		section: function(obj){
			if (obj.main) {
				go("#main > section").remove();
				go("#main").append(tmpl_section_main.parse(obj));
				if (!out.lockhist) out.history(obj.section);
			} else {
				go("#"+obj.section).replace(tmpl_section.parse(obj));
			}
		},
		make: function(obj){
			var frame = obj.block;
			if (!obj.block) return;
			for (var i = 0; i < frame.length; i++) if (typeof frame[i] == "object") {
				if (frame[i].section == "menu") {
					global.menu =  frame[i].block;
					document.title = obj.app + " - " + obj.mc;
					global.app = obj.app;
					global.mc = obj.mc;
					global.ver = obj.ver;
					if (!global.menu_id) global.menu_id = global.menu[0].value
					this.menu();
				} else
				if (frame[i].section == "content") {
					for (var n = 0; n < frame[i].block.length; n++) {
						go("#"+frame[i].block[n].id).replace(tmpl_content.parse(frame[i].block[n]));
					}
				} else {
					this.section(frame[i]);
				}
			}
			out.lockhist = false;
		},
		// обработка данных, полученных в пакете "pkg":"value"
		// блок разбирается на объекты по id и их value применяются к элементам шаблона на html странице
		value: function(obj){
			var frame = obj.block;
			if (!obj.block) return;
			for (var i = 0; i < frame.length; i++) if (typeof frame[i] == "object") {
				var el = go("#"+frame[i].id);
				if (el.length) {
					if (frame[i].html) {	// update placeholders in html template, like {{value.pMem}}
						global.value[frame[i].id] = frame[i].value
						el.html(frame[i].value);
					} else{
						el[0].value = frame[i].value;
						if (el[0].type == "range") go("#"+el[0].id+"-val").html(": "+el[0].value);
						// проверяем чекбоксы на значение вкл/выкл
						if (el[0].type == "checkbox") {
							// allow multiple types of TRUE value for checkboxes
							el[0].checked = (frame[i].value == "1" || frame[i].value == 1 || frame[i].value == true || frame[i].value == "true" );
							//console.debug("processing checkbox num: ", i, "val: ", frame[i].value);
						}
					}
				}
			}
		}
	};
	return out;
};

/**
 * rawdata callback - Stub function to handle rawdata messages from controller
 * Data could be sent from the controller via json_frame_custom(String("rawdata")) method
 * and handled in a custom user js script via redefined function
 */
function rawdata_cb(msg){
    console.log('Got raw data, redefine rawdata_cb(msg) func to handle it.', msg);
}

/**
 * User Callback for xload() function. Вызывается после завершения загрузки внешних данных, но
 * перед предачей объекта обработчику шаблона. Если коллбек возвращает false, то вызов шаблонизатора не происходит.
 * Может быть перенакрыта пользовательским скриптом для доп обработки загруженных данных
 */
function xload_cb(obj){
//    console.log('redefine xload_cb(obj) func to handle it.', msg);
	return true;
}

/**
 * @brief - Loads JSON objects via http request
 * @param {*} url - URI to load
 * @param {*} ok - callback on success
 * @param {*} err - callback on error
 */
function ajaxload(url, ok, err){
	var xhr = new XMLHttpRequest();
	xhr.overrideMimeType("application/json");
	xhr.responseType = 'json';
	xhr.open('GET', url, true);
	xhr.onreadystatechange = function(){
		if (xhr.readyState == 4 && xhr.status == "200") {
			ok && ok(xhr.response);
		} else if (xhr.status != "200"){
			err && err(xhr.status)
		}
	};
	xhr.send(null);
}

/**
 * 	"pkg":"xload" messages are used to make ajax requests for external JSON objects that could be used as data/interface templates
 * используется для загрузки контента/шаблонов из внешних источников - "флеш" контроллера, интернет ресурсы с погодой и т.п.,
 * объекты должны сохранять структуру как если бы они пришли от контроллера. Просмариваются рекурсивно все секции у которых есть ключ 'url',
 * этот урл запрашивается и результат записывается в ключ 'block' текущей секции. Ожидается что по URL будет доступен корректный JSON.
 * Результат передается в рендерер и встраивается в страницу /Vortigont/
 * @param { * } msg - framework content/interface object
 * @returns 
 */
function xload(msg){
    if (!msg.block){
        console.log('Message has no data block!');
        return;
    }

	console.log('Run deepfetch');
	deepfetch(msg.block).then(() => {
		 //console.log(msg);
		 if (xload_cb(msg)){
			var rdr = this.rdr = render();	// Interface rederer to pass updated objects to
			rdr.make(msg);
		 }
	})
}

/**
 * async function to traverse object and fetch all 'url' in sections,
 * this must be done in async mode till the last end, since there could be multiple recursive ajax requests
 * including in sections that were just fetched /Vortigont/
 * @param {*} obj - EmbUI data object
 */
async function deepfetch (obj) {
	for (let i = 0; i < obj.length; i++) if (typeof obj[i] == "object") {
		var section = obj[i];
		if (section.url){
			console.log('Fetching URL"' + section.url);
			await new Promise(next=> {
					ajaxload(section.url,
						function(response) {
							section['block'] = response;
							delete section.url;	// удаляем url из элемента т.к. работает рекурсия
							// пробегаемся рекурсивно по новым/вложенным объектам
							if (section.block && typeof section.block == "object") { 
								deepfetch(section.block).then(() => {
									//console.log("Diving deeper");
									next();
							   })
							} else {
								next();
							}
						},
						function(errstatus) {
							//console.log('Error loading external content');
							next();
						}
					);
			})
		} else if ( section.block && typeof section.block == "object" ){
			await new Promise(next=> {
				deepfetch(section.block).then(() => {
					next();
				})
			})			
		}
	}
}


window.addEventListener("load", function(ev){
	var rdr = this.rdr = render();
	var ws = this.ws = wbs("ws://"+location.host+"/ws");
	//var ws = this.ws = wbs("ws://embuitst/ws");
	ws.oninterface = function(msg) {
		rdr.make(msg);
	}
	ws.onvalue = function(msg){
		rdr.value(msg);
	}
	ws.onclose = ws.onerror = function(){
		ws.connect();
	}
	ws.connect();

	// any messages with "pkg":"rawdata" are handled here bypassing interface/value handlers
	ws.onrawdata = function(mgs){
		rawdata_cb(mgs);
	}

	// "pkg":"xload" messages are used to make ajax requests for external JSON objects that could be used as data/interface templates
	ws.onxload = function(mgs){
		xload(mgs);
	}

	var active = false, layout =  go("#layout");
	go("#menuLink").bind("click", function(){
		active = !active;
		if (active) layout.addClass("active");
		else layout.removeClass("active");
		return false;
	});
	go("#layout, #menu, #main").bind("click", function(){
		active = false;
		if (layout.contClass("active")[0]) {
			layout.removeClass("active");
			return false;
		}
	});
	
	// touch swipe left support
	let strX, strY, strT;
	go("body").bind("touchstart", function(e){
	  let t = e.changedTouches[0];
	  strX = t.pageX;
	  strY = t.pageY;
	  strT = new Date().getTime();
	}, {passive: true});

	go("body").bind("touchend", function(e){
	    let t = e.changedTouches[0],
	        elapsedT = new Date().getTime() - strT;
	    if (elapsedT <= 500 && Math.abs(t.pageY - strY) <= 75) {
	      if (!active && t.pageX - strX >= 50) {
		layout.addClass("active"); active = true;
	      }
	    }
	}, false);

}.bind(window));

window.addEventListener("popstate", function(e){
	if (e = e.state && e.state.hist) {
		rdr.lockhist = true;
		var data = {}; data[e] = null;
		ws.send_post(data);
	}
});
