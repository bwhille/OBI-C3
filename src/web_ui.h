#pragma once

const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>OBI Web</title>
  <style>
    :root{color-scheme:dark;--bg:#0b0e11;--panel:#171c22;--panel-strong:#1c232b;
      --line:#2a333d;--text:#f4f7f9;--muted:#9ba8b4;--yellow:#ffb000;--green:#42c983;
      --red:#ff5b5b;--blue:#58a6ff;--canvas:#0d1116;--table:#11171d;
      font-family:system-ui,-apple-system,"Segoe UI",sans-serif}
    :root[data-theme="light"]{color-scheme:light;--bg:#eef2f4;--panel:#ffffff;
      --panel-strong:#f8fafb;--line:#cbd5dc;--text:#18232c;--muted:#5f707d;
      --canvas:#f5f8fa;--table:#e8eef2}
    *{box-sizing:border-box}body{margin:0;background:radial-gradient(circle at
      10% 0%,#ffb00015,transparent 28rem),var(--bg);color:var(--text);transition:background .2s,color .2s}
    header,main{width:min(1080px,calc(100% - 28px));margin:auto}
    header{display:flex;align-items:center;justify-content:space-between;
      padding:30px 0 20px}.eyebrow{color:var(--yellow);font-size:.75rem;
      font-weight:800;letter-spacing:.13em;text-transform:uppercase;margin:0 0 3px}
    .header-actions{display:flex;align-items:center;gap:10px}
    footer{width:min(1080px,calc(100% - 28px));margin:24px auto 30px;
      padding-top:16px;border-top:1px solid var(--line);color:var(--muted);
      font-size:.78rem;line-height:1.5;text-align:center}
    h1{font-size:clamp(1.6rem,5vw,2.35rem);margin:0}h2{font-size:1rem;margin:0 0 15px}
    .panel{background:linear-gradient(145deg,var(--panel-strong),var(--panel));border:1px solid
      var(--line);border-radius:15px;box-shadow:0 14px 35px #0003}
    .badge{display:flex;align-items:center;gap:8px;padding:8px 12px;border:1px solid
      var(--line);border-radius:99px;color:var(--muted);font-size:.82rem}
    .dot{width:8px;height:8px;border-radius:50%;background:var(--red)}
    .online .dot{background:var(--green)}.controls{display:flex;gap:10px;
      padding:16px;flex-wrap:wrap}button,input{min-height:42px;border-radius:9px;
      border:1px solid var(--line);padding:0 14px;font:inherit}
    button{font-weight:750;cursor:pointer;background:var(--panel-strong);color:var(--text)}
    button.primary{background:var(--yellow);color:#17120a;border:0}
    button.secondary{background:#1f3448;border-color:#315474}
    button.danger{background:#b93636;border:0}button:disabled{opacity:.4;cursor:not-allowed}
    .notice{margin-top:13px;padding:12px 14px;border:1px solid var(--line);
      border-radius:9px;background:var(--panel)}.notice.ok{border-color:#42c98388}
    .notice.error{border-color:#ff5b5baa;color:#ffb2b2}.hidden{display:none}
    .summary{display:grid;grid-template-columns:repeat(5,1fr);gap:12px;margin-top:14px}
    .metric{padding:18px;display:grid;gap:8px}.metric span,dt,.muted,label{
      color:var(--muted);font-size:.8rem}.metric strong{font-size:1.35rem}
    .socbar{height:7px;background:#0d1116;border-radius:99px;overflow:hidden}
    .socbar i{display:block;height:100%;width:0;background:linear-gradient(90deg,var(--red),var(--yellow),var(--green));transition:width .3s}
    .content{display:grid;grid-template-columns:.85fr 1.15fr;gap:12px;margin-top:12px}
    .content>.panel,.danger,.wifi,.history,.chart-panel{padding:20px}.cells{display:grid;gap:5px}
    .cells div,.details div{display:flex;justify-content:space-between;gap:12px;
      border-bottom:1px solid var(--line);padding:10px 1px}.cells strong{color:#ffc84f}
    dl{margin:0}dd{margin:0;text-align:right}.mono{font-family:monospace;
      font-size:.75rem;word-break:break-all}.danger,.wifi{display:flex;
      align-items:center;justify-content:space-between;gap:20px;margin-top:12px}
    .danger{border-color:#ff5b5b66}.danger p,.wifi p,.history p{color:var(--muted);
      line-height:1.45;margin:5px 0 0}.wifi-form{display:flex;gap:8px;flex-wrap:wrap}
    .chart-panel,.history{margin-top:12px}.chart-head,.history-head{display:flex;align-items:center;
      justify-content:space-between;gap:12px;flex-wrap:wrap}.chart-wrap{position:relative;height:260px;margin-top:12px}
    canvas{width:100%;height:100%;display:block;background:var(--canvas);border:1px solid var(--line);border-radius:10px}
    .legend{display:flex;gap:14px;flex-wrap:wrap;color:var(--muted);font-size:.78rem}
    .legend span::before{content:"";display:inline-block;width:10px;height:3px;margin-right:6px;vertical-align:middle;background:var(--yellow)}
    .legend span:nth-child(2)::before{background:var(--blue)}.legend span:nth-child(3)::before{background:var(--green)}
    .table-wrap{overflow:auto;margin-top:12px;border:1px solid var(--line);border-radius:10px}
    table{border-collapse:collapse;width:100%;min-width:760px;font-size:.82rem}th,td{padding:10px 12px;
      border-bottom:1px solid var(--line);text-align:right;white-space:nowrap}th:first-child,td:first-child,
      th:nth-child(2),td:nth-child(2){text-align:left}th{color:var(--muted);background:var(--table);position:sticky;top:0}
    .empty{text-align:center!important;color:var(--muted);padding:24px}
    dialog{width:min(480px,calc(100% - 26px));border:1px solid var(--line);
      border-radius:14px;background:var(--panel);color:var(--text);padding:22px}
    dialog::backdrop{background:#000b}dialog input{width:100%;background:var(--canvas);
      color:var(--text);margin:8px 0 16px}.actions{display:flex;gap:8px;justify-content:flex-end}
    .theme-switch{display:flex;align-items:center;gap:8px;color:var(--muted);
      font-size:.78rem;cursor:pointer;white-space:nowrap}.theme-switch input{
      position:absolute;opacity:0;width:1px;height:1px}.switch-track{position:relative;
      width:46px;height:26px;border:1px solid var(--line);border-radius:99px;
      background:var(--panel-strong);transition:background .2s,border-color .2s}
    .switch-track::after{content:"";position:absolute;width:20px;height:20px;left:2px;top:2px;
      border-radius:50%;background:var(--muted);transition:transform .2s,background .2s}
    .theme-switch input:checked+.switch-track{background:var(--yellow);border-color:var(--yellow)}
    .theme-switch input:checked+.switch-track::after{transform:translateX(20px);background:#17120a}
    .theme-switch input:focus-visible+.switch-track{outline:2px solid var(--blue);outline-offset:3px}
    @media(max-width:900px){.summary{grid-template-columns:repeat(3,1fr)}}
    @media(max-width:760px){.summary{grid-template-columns:1fr 1fr}.content{
      grid-template-columns:1fr}.danger,.wifi{align-items:stretch;flex-direction:column}.chart-wrap{height:220px}}
  </style>
</head>
<body>
  <header>
    <div><p class="eyebrow">Open Battery Information</p><h1>Makita LXT Diagnose by HLTRP</h1></div>
    <div class="header-actions"><label class="theme-switch"><span id="themeLabel">Dunkel</span><input id="themeToggle" type="checkbox" role="switch" aria-label="Hellschema aktivieren"><span class="switch-track"></span></label><div id="badge" class="badge"><span class="dot"></span><span>Initialisiere</span></div></div>
  </header>
  <main>
    <section class="panel controls">
      <button id="read" class="primary">Akku auslesen</button>
      <button id="refresh" disabled>Messwerte aktualisieren</button>
      <button id="reload">Status aktualisieren</button>
      <button id="exportJson" class="secondary" disabled>Akku als JSON</button>
      <button id="exportCsv" class="secondary">Historie als CSV</button>
    </section>
    <div id="notice" class="notice hidden"></div>
    <section class="summary">
      <article class="panel metric"><span>Modell</span><strong id="model">–</strong></article>
      <article class="panel metric"><span>Zustand</span><strong id="state">–</strong></article>
      <article class="panel metric"><span>Packspannung</span><strong id="pack">–</strong></article>
      <article class="panel metric"><span>Zelldifferenz</span><strong id="difference">–</strong></article>
      <article class="panel metric"><span>Ladezustand (Schätzung)</span><strong id="soc">–</strong><div class="socbar"><i id="socFill"></i></div></article>
    </section>
    <section class="content">
      <article class="panel">
        <h2>Zellspannungen</h2>
        <div id="cells" class="cells">
          <div><span>Zelle 1</span><strong>–</strong></div>
          <div><span>Zelle 2</span><strong>–</strong></div>
          <div><span>Zelle 3</span><strong>–</strong></div>
          <div><span>Zelle 4</span><strong>–</strong></div>
          <div><span>Zelle 5</span><strong>–</strong></div>
        </div>
      </article>
      <article class="panel">
        <h2>Akkuinformationen</h2>
        <dl class="details">
          <div><dt>Statuscode</dt><dd id="code">–</dd></div>
          <div><dt>Ladezyklen</dt><dd id="cycles">–</dd></div>
          <div><dt>Kapazität</dt><dd id="capacity">–</dd></div>
          <div><dt>Herstellungsdatum</dt><dd id="date">–</dd></div>
          <div><dt>Zelltemperatur</dt><dd id="tempCell">–</dd></div>
          <div><dt>MOSFET-Temperatur</dt><dd id="tempMosfet">–</dd></div>
          <div><dt>ROM-ID</dt><dd id="rom" class="mono">–</dd></div>
          <div><dt>Firmware</dt><dd id="firmware">–</dd></div>
          <div><dt>Adresse</dt><dd id="ip">–</dd></div>
        </dl>
      </article>
    </section>
    <section class="panel chart-panel">
      <div class="chart-head"><div><h2>Messwertverlauf</h2><div class="legend"><span>Packspannung</span><span>Zelldifferenz ×100</span><span>Temperatur</span></div></div></div>
      <div class="chart-wrap"><canvas id="chart"></canvas></div>
    </section>
    <section class="panel history">
      <div class="history-head">
        <div><h2>Lokale Akkuhistorie</h2><p>Die letzten 50 Messungen werden ausschließlich in diesem Browser gespeichert.</p></div>
        <button id="clearHistory">Historie löschen</button>
      </div>
      <div class="table-wrap"><table><thead><tr><th>Zeit</th><th>Modell</th><th>Status</th><th>SoC</th><th>Pack</th><th>Differenz</th><th>Temperatur</th><th>Zyklen</th></tr></thead><tbody id="historyBody"></tbody></table></div>
    </section>
    <section class="panel danger">
      <div><h2>Fehler zurücksetzen</h2><p>Nur nach Prüfung und Reparatur der
        technischen Ursache ausführen. F0513 wird nur diagnostisch unterstützt.</p></div>
      <button id="reset" class="danger" disabled>Fehler löschen</button>
    </section>
    <section class="panel wifi">
      <div><h2>WLAN-Einrichtung</h2><p id="wifiState">–</p></div>
      <form id="wifiForm" class="wifi-form">
        <input name="ssid" placeholder="2,4-GHz-WLAN" required>
        <input name="password" type="password" placeholder="WLAN-Passwort">
        <button>Speichern</button>
      </form>
    </section>
  </main>
  <footer>Dieses Projekt dient ausschließlich zur Diagnose kompatibler Akkus. OBI-C3 ist ein unabhängiges Open-Source-Projekt und kein offizielles Makita®-Produkt. Der Einsatz erfolgt auf eigene Verantwortung.</footer>
  <dialog id="dialog">
    <form method="dialog">
      <h2>Fehler wirklich löschen?</h2>
      <p class="muted">Das Löschen repariert keinen defekten Akku. Zum
        Bestätigen <strong>RESET</strong> eingeben.</p>
      <input id="confirmation" autocomplete="off">
      <div class="actions"><button value="cancel">Abbrechen</button>
        <button id="confirm" value="default" class="danger">Jetzt löschen</button></div>
    </form>
  </dialog>
  <script>
    const $=s=>document.querySelector(s), els={
      badge:$("#badge"),notice:$("#notice"),read:$("#read"),refresh:$("#refresh"),
      reset:$("#reset"),dialog:$("#dialog"),confirmation:$("#confirmation"),
      exportJson:$("#exportJson"),exportCsv:$("#exportCsv"),chart:$("#chart"),
      themeToggle:$("#themeToggle")};
    const HISTORY_KEY="obi-c3-history-v1",THEME_KEY="obi-c3-theme-v1",MAX_HISTORY=50;
    let battery=null,history=loadHistory();
    const text=(id,value,suffix="")=>{$("#"+id).textContent=
      value===null||value===undefined||value===""?"–":value+suffix};
    const number=(value,digits)=>value===null||value===undefined?null:
      new Intl.NumberFormat("de-DE",{minimumFractionDigits:digits,
      maximumFractionDigits:digits}).format(value);
    function notice(message,type="ok"){els.notice.textContent=message;
      els.notice.className="notice "+type}
    function applyTheme(theme){document.documentElement.dataset.theme=theme;
      els.themeToggle.checked=theme==="light";$("#themeLabel").textContent=theme==="light"?"Hell":"Dunkel";
      els.themeToggle.setAttribute("aria-label",theme==="light"?"Dunkelschema aktivieren":"Hellschema aktivieren");
      localStorage.setItem(THEME_KEY,theme);drawChart()}
    async function api(path,options={}){const response=await fetch(path,options);
      const body=await response.json().catch(()=>({}));if(!response.ok)
      throw new Error(body.error||"HTTP "+response.status);return body}
    function estimateSoc(cells){
      if(!Array.isArray(cells)||!cells.length)return null;
      const v=cells.reduce((a,b)=>a+Number(b||0),0)/cells.length;
      const curve=[[3.20,0],[3.45,5],[3.60,10],[3.70,20],[3.75,35],[3.80,50],
        [3.90,65],[4.00,80],[4.10,92],[4.20,100]];
      if(v<=curve[0][0])return 0;if(v>=curve[curve.length-1][0])return 100;
      for(let i=1;i<curve.length;i++)if(v<=curve[i][0]){
        const a=curve[i-1],b=curve[i];return Math.round(a[1]+(v-a[0])/(b[0]-a[0])*(b[1]-a[1]));}
      return null;
    }
    function renderStatus(status){text("firmware",status.firmware_version);
      text("ip",status.ip);$("#wifiState").textContent=(status.wifi_mode==="ap"?
      "Einrichtungsmodus: ":"Verbunden mit ")+status.ssid+" · "+status.ip;
      els.badge.className="badge online";els.badge.lastElementChild.textContent=
      status.wifi_mode==="ap"?"Setup-WLAN":"Online"}
    function renderBattery(data,store=false){battery=data;if(!data||!data.valid)return;
      const soc=estimateSoc(data.cell_voltages);
      text("model",data.model);text("state",data.state);text("pack",
      number(data.pack_voltage,3)," V");text("difference",
      number(data.cell_voltage_difference,3)," V");text("soc",soc," %");
      $("#socFill").style.width=(soc||0)+"%";text("code",data.status_code);
      text("cycles",data.charge_count);text("capacity",number(data.capacity_ah,1)," Ah");
      text("date",data.manufacturing_date);text("tempCell",
      number(data.temperature_cell,1)," °C");text("tempMosfet",
      number(data.temperature_mosfet,1)," °C");text("rom",data.rom_id);
      const cells=document.querySelectorAll("#cells strong");
      (data.cell_voltages||[]).forEach((v,i)=>cells[i].textContent=number(v,3)+" V");
      els.refresh.disabled=false;els.reset.disabled=!!data.diagnostics_only;
      els.exportJson.disabled=false;if(store)storeHistory(data,soc)}
    function loadHistory(){try{const value=JSON.parse(localStorage.getItem(HISTORY_KEY)||"[]");
      return Array.isArray(value)?value:[]}catch{return[]}}
    function storeHistory(data,soc){
      const item={timestamp:new Date().toISOString(),model:data.model,state:data.state,
        status_code:data.status_code,soc,pack_voltage:data.pack_voltage,
        cell_voltage_difference:data.cell_voltage_difference,
        temperature_cell:data.temperature_cell,charge_count:data.charge_count,
        cell_voltages:data.cell_voltages};
      history.push(item);history=history.slice(-MAX_HISTORY);
      localStorage.setItem(HISTORY_KEY,JSON.stringify(history));renderHistory();drawChart();
    }
    function renderHistory(){const body=$("#historyBody");body.innerHTML="";
      if(!history.length){body.innerHTML='<tr><td class="empty" colspan="8">Noch keine Messungen gespeichert.</td></tr>';return}
      [...history].reverse().forEach(item=>{const row=document.createElement("tr");
        const values=[new Date(item.timestamp).toLocaleString("de-DE"),item.model,item.state,
          item.soc==null?"–":item.soc+" %",number(item.pack_voltage,3)+" V",
          number(item.cell_voltage_difference,3)+" V",number(item.temperature_cell,1)+" °C",
          item.charge_count];values.forEach(value=>{const cell=document.createElement("td");
          cell.textContent=value;row.appendChild(cell)});body.appendChild(row)})}
    function drawChart(){
      const canvas=els.chart,rect=canvas.getBoundingClientRect(),dpr=window.devicePixelRatio||1;
      canvas.width=Math.max(300,Math.round(rect.width*dpr));canvas.height=Math.max(180,Math.round(rect.height*dpr));
      const ctx=canvas.getContext("2d");ctx.scale(dpr,dpr);const w=rect.width,h=rect.height,p=34;
      const colors=getComputedStyle(document.documentElement);ctx.clearRect(0,0,w,h);
      ctx.strokeStyle=colors.getPropertyValue("--line");ctx.lineWidth=1;ctx.fillStyle=colors.getPropertyValue("--muted");ctx.font="11px system-ui";
      for(let i=0;i<=4;i++){const y=p+(h-2*p)*i/4;ctx.beginPath();ctx.moveTo(p,y);ctx.lineTo(w-p,y);ctx.stroke()}
      if(history.length<2){ctx.fillText("Mindestens zwei Messungen für den Verlauf erforderlich.",p,h/2);return}
      const series=[{key:"pack_voltage",color:"#ffb000"},{key:"cell_voltage_difference",color:"#58a6ff",factor:100},{key:"temperature_cell",color:"#42c983"}];
      const all=[];series.forEach(s=>history.forEach(v=>all.push(Number(v[s.key]||0)*(s.factor||1))));
      let min=Math.min(...all),max=Math.max(...all);if(min===max){min-=1;max+=1}const range=max-min;
      series.forEach(s=>{ctx.strokeStyle=s.color;ctx.lineWidth=2;ctx.beginPath();history.forEach((item,i)=>{
        const x=p+(w-2*p)*i/(history.length-1),value=Number(item[s.key]||0)*(s.factor||1),
          y=h-p-(h-2*p)*(value-min)/range;i?ctx.lineTo(x,y):ctx.moveTo(x,y)});ctx.stroke()});
      ctx.fillStyle="#9ba8b4";ctx.fillText(max.toFixed(1),4,p+4);ctx.fillText(min.toFixed(1),4,h-p+4)
    }
    function download(name,type,content){const blob=new Blob([content],{type}),url=URL.createObjectURL(blob),a=document.createElement("a");
      a.href=url;a.download=name;document.body.appendChild(a);a.click();a.remove();URL.revokeObjectURL(url)}
    function safeName(value){return String(value||"akku").replace(/[^a-z0-9_-]+/gi,"-")}
    function exportCurrent(){if(!battery||!battery.valid)return;const payload={exported_at:new Date().toISOString(),
      estimated_soc_percent:estimateSoc(battery.cell_voltages),battery};download("obi-"+safeName(battery.model)+".json",
      "application/json;charset=utf-8",JSON.stringify(payload,null,2))}
    function exportHistory(){if(!history.length){notice("Es ist noch keine Historie vorhanden.","error");return}
      const header=["Zeit","Modell","Zustand","Statuscode","SoC Prozent","Packspannung V","Zelldifferenz V","Temperatur C","Ladezyklen","Zelle1 V","Zelle2 V","Zelle3 V","Zelle4 V","Zelle5 V"];
      const rows=history.map(v=>[v.timestamp,v.model,v.state,v.status_code,v.soc,v.pack_voltage,
        v.cell_voltage_difference,v.temperature_cell,v.charge_count,...(v.cell_voltages||[])]);
      const csv=[header,...rows].map(row=>row.map(value=>'"'+String(value??"").replace(/"/g,'""')+'"').join(";")).join("\r\n");
      download("obi-historie.csv","text/csv;charset=utf-8","\ufeff"+csv)}
    async function load(){try{const data=await api("/api/status");
      renderStatus(data.status);renderBattery(data.battery)}catch(e){notice(e.message,"error")}}
    async function run(path){document.body.style.cursor="progress";try{
      const result=await api(path,{method:"POST"});
      renderBattery(result.battery,true);notice("Akku erfolgreich ausgelesen.")}
      catch(e){notice(e.message,"error")}finally{document.body.style.cursor=""}}
    els.read.onclick=()=>run("/api/read");els.refresh.onclick=()=>run("/api/read/live");
    $("#reload").onclick=load;els.exportJson.onclick=exportCurrent;els.exportCsv.onclick=exportHistory;
    $("#clearHistory").onclick=()=>{if(!confirm("Lokale Messhistorie wirklich löschen?"))return;
      history=[];localStorage.removeItem(HISTORY_KEY);renderHistory();drawChart();notice("Historie gelöscht.")};
    els.reset.onclick=()=>{els.confirmation.value="";els.dialog.showModal()};
    $("#confirm").onclick=async event=>{event.preventDefault();
      if(els.confirmation.value!=="RESET"){notice("Bitte RESET exakt eingeben.","error");
      return}els.dialog.close();const form=new URLSearchParams({confirmation:"RESET"});
      try{const result=await api("/api/errors/reset",{method:"POST",
      headers:{"Content-Type":"application/x-www-form-urlencoded"},body:form});
      renderBattery(result.battery,true);notice(result.message+" Vorher: "+
      result.before.state+"/"+result.before.status_code+", nachher: "+
      result.after.state+"/"+result.after.status_code+".")}catch(e){notice(e.message,"error")}};
    $("#wifiForm").onsubmit=async event=>{event.preventDefault();
      try{const body=new URLSearchParams(new FormData(event.target));
      const result=await api("/api/wifi",{method:"POST",headers:
      {"Content-Type":"application/x-www-form-urlencoded"},body});
      notice(result.message)}catch(e){notice(e.message,"error")}};
    els.themeToggle.onchange=()=>applyTheme(els.themeToggle.checked?"light":"dark");
    applyTheme(localStorage.getItem(THEME_KEY)==="light"?"light":"dark");
    window.addEventListener("resize",()=>requestAnimationFrame(drawChart));
    renderHistory();drawChart();load();
  </script>
</body>
</html>
)HTML";