# ── Infrastructure Monitor ─────────────────────────────────────────────────
# Usage: .\monitor.ps1
# Edit $TARGETS below to match your actual server ports.

$TARGETS = @(
    # Game servers (.103)
    @{ label = "CS2 #1";    host = "185.113.141.103"; port = 27015 },
    @{ label = "CS2 #2";    host = "185.113.141.103"; port = 27016 },
    @{ label = "CS2 #3";    host = "185.113.141.103"; port = 27017 },
    @{ label = "CS2 #4";    host = "185.113.141.103"; port = 27018 },
    @{ label = "Wings";     host = "185.113.141.103"; port = 8080  },
    @{ label = "SSH (.103)";host = "185.113.141.103"; port = 22    },

    # Panel server (.37)
    @{ label = "Panel";     host = "185.113.143.37";  port = 80    },
    @{ label = "SSH (.37)"; host = "185.113.143.37";  port = 22    }
)

$INTERVAL    = 10    # seconds between full sweeps
$PAPING_PATH = "paping"   # just 'paping' since it's on PATH; change to full path if needed
$LATENCY_WARN  = 80  # ms — yellow above this
$LATENCY_CRIT  = 200 # ms — red above this
$PING_COUNT  = 3     # pings per target per sweep

# ── Helpers ────────────────────────────────────────────────────────────────

function Get-Latency($host_, $port) {
    $output = & $PAPING_PATH $host_ -p $port -c $PING_COUNT -t 2000 2>&1 | Out-String
    # Parse "time=XXXms" from paping output
    $times = [regex]::Matches($output, 'time=([\d.]+)ms') | ForEach-Object { [float]$_.Groups[1].Value }
    if ($times.Count -eq 0) { return $null }
    return [math]::Round(($times | Measure-Object -Average).Average, 1)
}

function Write-Status($label, $host_, $port, $ms) {
    $pad = $label.PadRight(12)
    $addr = "${host_}:${port}".PadRight(26)

    if ($null -eq $ms) {
        Write-Host "  $pad  $addr  " -NoNewline
        Write-Host "DOWN     " -ForegroundColor Red -NoNewline
        Write-Host "timeout" -ForegroundColor DarkRed
    } elseif ($ms -ge $LATENCY_CRIT) {
        Write-Host "  $pad  $addr  " -NoNewline
        Write-Host "CRIT     " -ForegroundColor Red -NoNewline
        Write-Host "${ms}ms" -ForegroundColor Red
    } elseif ($ms -ge $LATENCY_WARN) {
        Write-Host "  $pad  $addr  " -NoNewline
        Write-Host "WARN     " -ForegroundColor Yellow -NoNewline
        Write-Host "${ms}ms" -ForegroundColor Yellow
    } else {
        Write-Host "  $pad  $addr  " -NoNewline
        Write-Host "OK       " -ForegroundColor Green -NoNewline
        Write-Host "${ms}ms" -ForegroundColor DarkGreen
    }
}

function Write-Header {
    $ts = Get-Date -Format "HH:mm:ss"
    Write-Host ""
    Write-Host "  ── Infrastructure Monitor [$ts] ──────────────────────────" -ForegroundColor DarkGray
    Write-Host ("  {0}  {1}  {2}  {3}" -f "Service".PadRight(12), "Endpoint".PadRight(26), "Status".PadRight(9), "Latency") -ForegroundColor DarkGray
    Write-Host "  $("─" * 62)" -ForegroundColor DarkGray
}

# ── Main loop ──────────────────────────────────────────────────────────────

Write-Host "  Starting monitor — Ctrl+C to stop" -ForegroundColor DarkYellow
Write-Host "  Interval: ${INTERVAL}s  |  Warn: >${LATENCY_WARN}ms  |  Crit: >${LATENCY_CRIT}ms" -ForegroundColor DarkGray

while ($true) {
    # Run all pings in parallel as background jobs
    $jobs = $TARGETS | ForEach-Object {
        $t = $_
        Start-Job -ScriptBlock {
            param($paping, $host_, $port, $count)
            $output = & $paping $host_ -p $port -c $count -t 2000 2>&1 | Out-String
            $times = [regex]::Matches($output, 'time=([\d.]+)ms') | ForEach-Object { [float]$_.Groups[1].Value }
            if ($times.Count -eq 0) { return $null }
            return [math]::Round(($times | Measure-Object -Average).Average, 1)
        } -ArgumentList $PAPING_PATH, $t.host, $t.port, $PING_COUNT
    }

    # Wait for all jobs to finish
    $null = $jobs | Wait-Job

    # Collect results
    $results = $jobs | ForEach-Object { Receive-Job $_ }
    $jobs | Remove-Job

    # Render
    Clear-Host
    Write-Header

    $down = 0
    for ($i = 0; $i -lt $TARGETS.Count; $i++) {
        $t  = $TARGETS[$i]
        $ms = $results[$i]
        if ($null -eq $ms) { $down++ }
        Write-Status $t.label $t.host $t.port $ms
    }

    Write-Host "  $("─" * 62)" -ForegroundColor DarkGray
    if ($down -gt 0) {
        Write-Host "  $down service(s) DOWN — next sweep in ${INTERVAL}s" -ForegroundColor Red
    } else {
        Write-Host "  All services OK — next sweep in ${INTERVAL}s" -ForegroundColor DarkGray
    }

    Start-Sleep -Seconds $INTERVAL
}
